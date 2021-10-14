#include "Framework.h"
#include "MeshRender.h"

MeshRender::MeshRender(Shader * shader, Mesh * mesh)
	: mesh(mesh)
{
	Pass(0);
	mesh->SetShader(shader);

	instanceBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MESH_INSTANCE, sizeof(Color), 2, true);
}

MeshRender::~MeshRender()
{
	SafeDelete(instanceBuffer);
	SafeDelete(instanceColorBuffer);
	SafeDelete(mesh);

	for (Transform* transform : transforms)
		SafeDelete(transform);
}

//TODO
void MeshRender::Update()
{
}

void MeshRender::Render()
{
}

Transform * MeshRender::AddTransform()
{
	return nullptr;
}

void MeshRender::UpdateTransforms()
{
}

void MeshRender::SetColor(UINT instance, Color & color)
{
}
