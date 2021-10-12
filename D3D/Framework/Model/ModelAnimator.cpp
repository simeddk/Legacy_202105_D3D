#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(Shader * shader)
	: shader(shader)
{
	model = new Model();
	transform = new Transform(shader);

	frameBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc));
	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc));

	//Create ComputeShader
	{
		computeShader = new Shader(L"16_GetBones.fxo");

		sComputeWorld = computeShader->AsMatrix("World");

		sComputeFrameBuffer = computeShader->AsConstantBuffer("CB_AnimationFrames");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendingFrames");
		sComputeTransformSRV = computeShader->AsSRV("TransformMap");

		computeBoneBuffer = new StructuredBuffer(nullptr, sizeof(Matrix), MAX_MODEL_TRANSFORMS, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sComputeInputBoneBuffer = computeShader->AsSRV("InputBones");
		sComputeOutputBoneBuffer = computeShader->AsUAV("OutputBones");
	}
	 
}

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);
	SafeDelete(transform);

	SafeDeleteArray(clipTransforms);
	SafeRelease(texture);
	SafeRelease(transformSRV);

	SafeDelete(frameBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(computeShader);
	SafeDelete(computeBoneBuffer);
}

void ModelAnimator::Update()
{
	if (texture == nullptr)
	{
		SetShader(shader, true);
		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform();

		computeBoneBuffer->CopyToInput(bones);
	}

	if (blendDesc.Mode == 0)
		UpdateAnimationFrame();
	else
		UpdateBledningFrame();

	frameBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeWorld->SetMatrix(transform->World());

		sComputeFrameBuffer->SetConstantBuffer(frameBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());
		sComputeTransformSRV->SetResource(transformSRV);

		sComputeInputBoneBuffer->SetResource(computeBoneBuffer->SRV());
		sComputeOutputBoneBuffer->SetUnorderedAccessView(computeBoneBuffer->UAV());

		shader->Dispatch(0, 0, 1, 1, 1);
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

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelAnimator::UpdateAnimationFrame()
{
	TweenDesc& desc = tweenDesc;
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

void ModelAnimator::UpdateBledningFrame()
{
	BlendDesc& desc = blendDesc;

	for (UINT i = 0; i < 3; i++)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clips[i].Clip);
		
		desc.Clips[i].RunningTime += Time::Delta();

		float time = 1.0f / clip->FrameRate() / desc.Clips[i].Speed;

		if (desc.Clips[i].Time >= 1.0f)
		{
			desc.Clips[i].RunningTime = 0.0f;

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

void ModelAnimator::PlayTweenMode(UINT clip, float speed, float takeTime)
{
	tweenDesc.TakeTime = takeTime;
	tweenDesc.Next.Clip = clip;
	tweenDesc.Next.Speed = speed;
}

void ModelAnimator::PlayBlendMode(UINT clip1, UINT clip2, UINT clip3)
{
	blendDesc.Mode = 1;

	blendDesc.Clips[0].Clip = clip1;
	blendDesc.Clips[1].Clip = clip2;
	blendDesc.Clips[2].Clip = clip3;
}

void ModelAnimator::SetBlendAlpha(float alpha)
{
	alpha = Math::Clamp<float>(alpha, 0.0f, 2.0f);

	blendDesc.Alpha = alpha;
}

void ModelAnimator::SetShader(Shader * shader, bool bFirst)
{
	this->shader = shader;

	if (bFirst == false)
	{
		SafeDelete(transform);
		transform = new Transform(shader);
	}

	sTransformSRV = shader->AsSRV("TransformMap");

	sFrameBuffer = shader->AsConstantBuffer("CB_AnimationFrames");
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendFrames");

	for (ModelMesh* mesh : model->Meshes())
		mesh->SetShader(shader);

}

void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelAnimator::GetAttachBones(Matrix * matrix)
{
	computeBoneBuffer->CopyFromOutput(matrix);
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
