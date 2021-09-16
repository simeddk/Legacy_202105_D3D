#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Shader * shader)
	: Renderer(shader)
{
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(diffuseMap);
}

void Mesh::Update()
{
	Super::Update();
}

void Mesh::Render()
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
	{
		Create();
		
		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexMesh));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}

	Super::Render();

	if (diffuseMap != nullptr)
		sDiffuseMap->SetResource(diffuseMap->SRV());

	shader->DrawIndexed(0, Pass(), indexCount);
}


void Mesh::DiffuseMap(wstring file)
{
	SafeDelete(diffuseMap);

	diffuseMap = new Texture(file);
}


