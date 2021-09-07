#include "stdafx.h"
#include "IndeBufferDemo.h"

void IndeBufferDemo::Initialize()
{
	shader = new Shader(L"04_World.fxo");
	
	D3DXMatrixIdentity(&world);

	//Create VertexBuffer
	{
		vertices[0].Position = Vector3(-0.5f, -0.5f, 0);
		vertices[1].Position = Vector3(-0.5f, +0.5f, 0);
		vertices[2].Position = Vector3(+0.5f, -0.5f, 0);
		vertices[3].Position = Vector3(+0.5f, +0.5f, 0);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Create IndexBuffer
	{
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}
}

void IndeBufferDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void IndeBufferDemo::Update()
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


void IndeBufferDemo::Render()
{
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
	shader->AsVector("Color")->SetFloatVector(Color(0, 0, 1, 1));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;

	shader->DrawIndexed(0, pass, 6);
}



