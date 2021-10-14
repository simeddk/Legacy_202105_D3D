#include "stdafx.h"
#include "InstanceDemo.h"

void InstanceDemo::Initialize()
{
	Context::Get()->GetCamera()->Position(0, 0, -2);

	shader = new Shader(L"18_Instance.fxo");

	perFrame = new PerFrame(shader);

	material = new Material(shader);
	material->DiffuseMap(L"Box.png");

	for (UINT i = 0; i < 5000; i++)
	{
		transforms[i] = new Transform(shader);

		//transforms[i]->Position(Math::RandomVec3(-30, 30));
		//transforms[i]->Scale(Math::RandomVec3(1.0f, 2.5f));
		//transforms[i]->RotationDegree(Math::RandomVec3(-180, 180));

		worlds[i] = transforms[i]->World();

		colors[i] = Color(0, 0, 0, 1);
		if (i % 3 == 0)
			colors[i] = Math::RandomColor4();
	}

	CreateMesh();

	instanceBuffer = new VertexBuffer(worlds, 5000, sizeof(Matrix), 1);
	colorInstanceBuffer = new VertexBuffer(colors, 5000, sizeof(Color), 2);
}

void InstanceDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(perFrame);
	SafeDelete(material);

	for (UINT i = 0; i < 5000; i++)
		SafeDelete(transforms[i]);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);
	SafeDelete(instanceBuffer);
	SafeDelete(colorInstanceBuffer);
}

void InstanceDemo::Update()
{
	perFrame->Update();
}


void InstanceDemo::Render()
{
	perFrame->Render();
	material->Render();

	vertexBuffer->Render();
	indexBuffer->Render();
	instanceBuffer->Render();
	colorInstanceBuffer->Render();

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->DrawIndexedInstanced(0, 1, indices.size(), 5000);

	
}

void InstanceDemo::CreateMesh()
{
	float w = 0.5f, h = 0.5f, d = 0.5f;

	//Front
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 0, 1, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 1, 1, 0, 0, -1));

	//Back
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 0, 0, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 1, 0, 0, 0, 1));

	//Top
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 1, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 1, 0, 1, 0));

	//Bottom
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 0, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 0, 0, -1, 0));

	//Left
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 0, 1, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 1, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, -1, 0, 0));

	//Right
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 0, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 1, 1, 1, 0, 0));

	indices = 
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(Mesh::VertexMesh));
	indexBuffer = new IndexBuffer(&indices[0], indices.size());
}



