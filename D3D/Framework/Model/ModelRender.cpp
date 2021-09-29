#include "Framework.h"
#include "ModelRender.h"

ModelRender::ModelRender(Shader * shader)
	: shader(shader)
{
	model = new Model();

	transform = new Transform();
	//transform->Scale(10, 10, 10);
}

ModelRender::~ModelRender()
{
	SafeDelete(model);
	SafeDelete(transform);
}

void ModelRender::Update()
{
	if (bRead == true)
	{
		bRead = false;

		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		UpdateTransforms();
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Update();
}

void ModelRender::Render()
{
	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->SetTransform(transform);
		mesh->Render();
	}
}

void ModelRender::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelRender::ReadMaterial(wstring file)
{
	bRead = true;

	model->ReadMaterial(file);
}

void ModelRender::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelRender::UpdateTransforms()
{
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->BoneByIndex(i);
		transforms[i] = bone->Transform();
	}

	for (ModelMesh* mesh : model->Meshes())
		mesh->Transforms(transforms);
}
