#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
	SafeDelete(perFrame);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void Mesh::Update()
{
	perFrame->Update();
}

void Mesh::Render(UINT instanceCount)
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
	{
		Create();
		
		vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexMesh));
		indexBuffer = new IndexBuffer(indices, indexCount);
	}

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexedInstanced(0, pass, indexCount, instanceCount);
}

void Mesh::SetShader(Shader * shader)
{
	this->shader = shader;

	SafeDelete(perFrame);
	perFrame = new PerFrame(shader);
}


