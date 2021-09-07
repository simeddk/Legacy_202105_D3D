#include "stdafx.h"
#include "VertexTriangleDemo.h"

void VertexTriangleDemo::Initialize()
{
	shader = new Shader(L"03_LinePrimitive.fxo");

	//Tri1
	vertices[0].Position = Vector3(-0.5f, 0.0f, 0);
	vertices[1].Position = Vector3(0.0f, +0.5f, 0);
	vertices[2].Position = Vector3(+0.5f, 0.0f, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 3;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

	//Tri2
	vertices2[0].Position = Vector3(-0.5f, 0.0f - 0.5f, 0);
	vertices2[1].Position = Vector3(0.0f, +0.5f - 0.5f, 0);
	vertices2[2].Position = Vector3(+0.5f, 0.0f - 0.5f, 0);

	subResource = { 0 };
	subResource.pSysMem = vertices2;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
}

void VertexTriangleDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(vertexBuffer2);
}

void VertexTriangleDemo::Update()
{
}


void VertexTriangleDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Pass
	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);

	pass = Math::Clamp<UINT>(pass, 0, 3);

	//VectorParam Test
	static Color color = Color(1, 1, 1, 1);
	ImGui::ColorEdit3("Color", color);
	shader->AsVector("ImGuiColor")->SetFloatVector(color);

	shader->Draw(0, 0, 3);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	shader->Draw(0, pass, 3);
}



