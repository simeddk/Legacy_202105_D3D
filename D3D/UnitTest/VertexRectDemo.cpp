#include "stdafx.h"
#include "VertexRectDemo.h"

void VertexRectDemo::Initialize()
{
	shader = new Shader(L"03_LinePrimitive.fxo");

	//Tri1
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexRectDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void VertexRectDemo::Update()
{
}


void VertexRectDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Pass
	static UINT pass = 4;
	ImGui::InputInt("Pass", (int*)&pass);

	pass = Math::Clamp<UINT>(pass, 0, 4);

	//VectorParam Test
	static Color color = Color(1, 1, 1, 1);
	ImGui::ColorEdit3("Color", color);
	shader->AsVector("ImGuiColor")->SetFloatVector(color);

	shader->Draw(0, pass, 6);
}



