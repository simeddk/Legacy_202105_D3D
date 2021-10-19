#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc) * MAX_MODEL_INSTANCE);

	sTransformSRV = shader->AsSRV("TransformMap");

	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrames");
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendFrames");

	//Create ComputeShader
	{
		computeShader = new Shader(L"20_GetBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrames");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendFrames");
		sComputeTransformSRV = computeShader->AsSRV("TransformMap");

		computeBoneBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones");

		computeWorldBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_INSTANCE);
		sComputeInputWorldBuffer = computeShader->AsSRV("InputWorlds");

		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &texture));

		computeOutputBuffer = new TextureBuffer(texture);
		sComputeOutputBuffer = computeShader->AsUAV("Output");


		attachBones = new Matrix*[MAX_MODEL_INSTANCE];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
			attachBones[i] = new Matrix[MAX_MODEL_TRANSFORMS];
	}
	 
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);

	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceBuffer);
	SafeDelete(instanceColorBuffer);

	SafeRelease(texture);
	SafeDeleteArray(clipTransforms);
	SafeRelease(transformSRV);

	SafeDelete(frameBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(computeShader);
	SafeDelete(computeBoneBuffer);
	SafeDelete(computeWorldBuffer);
	SafeDelete(computeOutputBuffer);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		SafeDeleteArray(attachBones[i]);
	SafeDeleteArray(attachBones);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		computeBoneBuffer->CopyToInput(bones);
	}

	for (UINT i = 0; i < transforms.size(); i++)
	{
		if (blendDesc[i].Mode == 0)
			UpdateAnimationFrame(i);
		else
			UpdateBlendFrame(i);
	}

	frameBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		sComputeTransformSRV->SetResource(transformSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->SRV());
		sComputeInputWorldBuffer->SetResource(computeWorldBuffer->SRV());
		sComputeOutputBuffer->SetUnorderedAccessView(computeOutputBuffer->UAV());

		computeShader->Dispatch(0, 0, 1, MAX_MODEL_INSTANCE, 1);

		ID3D11Texture2D* texture = computeOutputBuffer->CopyFromOutput();
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
		{
			for (UINT i = 0; i < transforms.size(); i++)
			{
				BYTE* start = (BYTE*)subResource.pData + (i * subResource.RowPitch);

				memcpy(attachBones[i], start, sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
			}
		}
		D3D::GetDC()->Unmap(texture, 0);
	}
	frameTime = fmod(frameTime, (1.0f / frameRate));

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelAnimator::Render()
{
	sFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
	
	sTransformSRV->SetResource(transformSRV);

	instanceBuffer->Render();
	instanceColorBuffer->Render();

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render(transforms.size());
	}
}


void ModelAnimator::UpdateAnimationFrame(UINT instance)
{
	TweenDesc& desc = tweenDesc[instance];
	ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

	desc.Curr.RunningTime += Time::Delta();

	float time = 1.0f / clip->FrameRate() / desc.Curr.Speed;

	if (desc.Curr.Time >= 1.0f)
	{
		desc.Curr.RunningTime = 0.0f;

		desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
		desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount();
	}

	desc.Curr.Time = desc.Curr.RunningTime / time;

	if (desc.Next.Clip > -1)
	{
		ModelClip* nextClip = model->ClipByIndex(desc.Next.Clip);

		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime;

		if (desc.TweenTime >= 1.0f)
		{
			desc.Curr = desc.Next;

			desc.Next.Clip = -1;
			desc.Next.CurrFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0;
			desc.ChangeTime = 0.0f;
			desc.TweenTime = 0.0f;
		}
		else
		{
			desc.Next.RunningTime += Time::Delta();

			float time = 1.0f / clip->FrameRate() / desc.Next.Speed;

			if (desc.Next.Time >= 1.0f)
			{
				desc.Next.RunningTime = 0.0f;

				desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
				desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount();
			}

			desc.Next.Time = desc.Next.RunningTime / time;
		}

	}
}

void ModelAnimator::UpdateBlendFrame(UINT instance)
{
	BlendDesc& desc = blendDesc[instance];

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clips[i].Clip);

		desc.Clips[i].RunningTime += Time::Delta();

		float time = 1.0f / clip->FrameRate() / desc.Clips[i].Speed;

		if (desc.Clips[i].Time >= 1.0f)
		{
			desc.Clips[i].RunningTime = 0;

			desc.Clips[i].CurrFrame = (desc.Clips[i].CurrFrame + 1) % clip->FrameCount();
			desc.Clips[i].NextFrame = (desc.Clips[i].CurrFrame + 1) % clip->FrameCount();
		}
		desc.Clips[i].Time = desc.Clips[i].RunningTime / time;
	}

	
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::PlayTweenMode(UINT instance, UINT clip, float speed, float takeTime)
{
	tweenDesc[instance].TakeTime = takeTime;
	tweenDesc[instance].Next.Clip = clip;
	tweenDesc[instance].Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT instance, UINT clip1, UINT clip2, UINT clip3)
{
	blendDesc[instance].Mode = 1;
	blendDesc[instance].Clips[0].Clip = clip1;
	blendDesc[instance].Clips[1].Clip = clip2;
	blendDesc[instance].Clips[2].Clip = clip3;
}

void ModelAnimator::SetBlendAlpha(UINT instance, float alpha)
{
	alpha = Math::Clamp<float>(alpha, 0.0f, 2.0f);

	blendDesc[instance].Alpha = alpha;
}


void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

Transform * ModelAnimator::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void ModelAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	computeWorldBuffer->CopyToInput(worlds);

	D3D11_MAPPED_SUBRESOURCE subResource;
	//Instance Transform
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);

	//Instance Color
	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MODEL_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void ModelAnimator::SetColor(UINT instance, Color & color)
{
	colors[instance] = color;
}

void ModelAnimator::GetAttachBones(UINT instance, Matrix * matrix)
{
	memcpy(matrix, attachBones[instance], sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
}

void ModelAnimator::CreateTexture()
{
	clipTransforms = new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
		CreateClipTransform(i);

	//CreateTexture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		int pageSize = sizeof(Matrix) * MAX_MODEL_TRANSFORMS * MAX_MODEL_KEYFRAMES;
		
		void* p = VirtualAlloc(nullptr, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;

			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				void* temp = (BYTE*)p + MAX_MODEL_TRANSFORMS * k * sizeof(Matrix) + start;
				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix));
			}
		}

		int a = model->ClipCount();

		D3D11_SUBRESOURCE_DATA* subResource = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE*)p + c * pageSize;
			subResource[c].pSysMem = temp;
			subResource[c].SysMemPitch = sizeof(Matrix) * MAX_MODEL_TRANSFORMS;
			subResource[c].SysMemSlicePitch = pageSize;
			
		}
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResource, &texture));

		SafeDeleteArray(subResource);
		VirtualFree(p, 0, MEM_RELEASE);
	}

	//CreateSRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &transformSRV));
	}

}

void ModelAnimator::CreateClipTransform(UINT index)
{
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(index);

	for (UINT f = 0; f < clip->FrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix inv = bone->Transform();//L C

			D3DXMatrixInverse(&inv, nullptr, &inv);

			Matrix parent;
			int parentIndex = bone->ParentIndex();

			if (parentIndex < 0)
				D3DXMatrixIdentity(&parent);
			else
				parent = bones[parentIndex];

			Matrix animation;

			ModelKeyFrame* frame = clip->Keyframe(bone->Name());

			if (frame != nullptr)
			{
				ModelKeyFrameData& data = frame->Transforms[f];

				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
			{
				D3DXMatrixIdentity(&animation);
			}

			bones[b] = animation * parent;
			clipTransforms[index].Transform[f][b] = inv * bones[b];
		}
	}
}
