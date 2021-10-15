#include "Framework.h"
#include "ModelRender.h"

ModelRender::ModelRender(Shader * shader)
	: shader(shader)
{
	model = new Model();

	sTransformSRV = shader->AsSRV("TransformMap");

	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);
}

ModelRender::~ModelRender()
{
	SafeDelete(model);

	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceBuffer);
	SafeDelete(instanceColorBuffer);

	SafeRelease(texture);
	SafeRelease(transformSRV);
}

void ModelRender::Update()
{
	if (texture == nullptr)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelRender::Render()
{
	instanceBuffer->Render();
	instanceColorBuffer->Render();
	sTransformSRV->SetResource(transformSRV);

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render();
	}
}

void ModelRender::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelRender::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelRender::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelRender::CreateTexture()
{
	//CreateTexture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_INSTANCE;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;

		//SubResource Data
		Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
		{
			for (UINT b = 0; b < model->BoneCount(); b++)
			{
				ModelBone* bone = model->BoneByIndex(b);

				Matrix parent;
				int parentIndex = bone->ParentIndex();

				if (parentIndex < 0)
					D3DXMatrixIdentity(&parent);
				else
					parent = bones[parentIndex];

				Matrix matrix = bone->Transform();
				bones[b] = parent;
				boneTransforms[i][b] = matrix * bones[b];
			} //for(b)
		}//for(i)

		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = boneTransforms;
		subResource.SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
		subResource.SysMemSlicePitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix) * MAX_MODEL_INSTANCE;

		D3D::GetDevice()->CreateTexture2D(&desc, &subResource, &texture);

	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &transformSRV));
	}
}

Transform * ModelRender::AddTransform()
{
	Transform* transform = new Transform();

	transforms.push_back(transform);
	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void ModelRender::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

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

void ModelRender::SetColor(UINT instance, Color & color)
{
	colors[instance] = color;
}
