#include "stdafx.h"
#include "VertexPrimitiveDemo.h"

void VertexPrimitiveDemo::Initialize()
{
	shader = new Shader(L"03_LinePrimitive.fxo");

	vertices[0].Position = Vector3(0, 0, 0);
	vertices[1].Position = Vector3(1, 0, 0);

	vertices[2].Position = Vector3(0, 0.5f, 0);
	vertices[3].Position = Vector3(1, 0.5f, 0);

	vertices[4].Position = Vector3(0, -0.5f, 0);
	vertices[5].Position = Vector3(1, -0.5f, 0);


	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexPrimitiveDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void VertexPrimitiveDemo::Update()
{
}


void VertexPrimitiveDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	
	//PrimitiveTopology Test
	static bool bStrip = false;
	ImGui::Checkbox("Strip", &bStrip);
	D3D11_PRIMITIVE_TOPOLOGY topology[2] = 
	{
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	};
	D3D::GetDC()->IASetPrimitiveTopology(bStrip ? topology[1] : topology[0]);

	//StartLocation Test
	static UINT startLoction = 0;
	ImGui::SliderInt("StartLocation", (int*)&startLoction, 0, 6);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);

	pass = Math::Clamp<UINT>(pass, 0, 3);

	//VectorParam Test
	static Color color = Color(1, 1, 1, 1);
	ImGui::ColorEdit3("Color", color);
	shader->AsVector("ImGuiColor")->SetFloatVector(color);

	shader->Draw(0, pass, 6, startLoction);
}



