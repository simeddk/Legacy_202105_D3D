#include "stdafx.h"
#include "WorldDemo.h"

void WorldDemo::Initialize()
{
	shader = new Shader(L"04_World.fxo");

	//Tri1
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0);

	/*vertices[0].Position = Vector3(-0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[1].Position = Vector3(-0.5f + 0.5f, +0.5f + 0.5f, 0);
	vertices[2].Position = Vector3(+0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[3].Position = Vector3(+0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[4].Position = Vector3(-0.5f + 0.5f, +0.5f + 0.5f, 0);
	vertices[5].Position = Vector3(+0.5f + 0.5f, +0.5f + 0.5f, 0);*/

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

	D3DXMatrixIdentity(&world);
}

void WorldDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void WorldDemo::Update()
{
	if (Keyboard::Get()->Press(VK_LSHIFT))
	{
		if (Keyboard::Get()->Press('D'))
			world._11 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			world._11 -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			world._22 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			world._22 -= 2.0f * Time::Delta();

		return;
	}


	if (Keyboard::Get()->Press('D'))
		world._41 += 2.0f * Time::Delta();
	else if (Keyboard::Get()->Press('A'))
		world._41 -= 2.0f * Time::Delta();

	if (Keyboard::Get()->Press('W'))
		world._42 += 2.0f * Time::Delta();
	else if (Keyboard::Get()->Press('S'))
		world._42 -= 2.0f * Time::Delta();
}


void WorldDemo::Render()
{
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;

	shader->Draw(0, pass, 6);
}



