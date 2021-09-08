#include "stdafx.h"
#include "GirdDemo.h"

void GirdDemo::Initialize()
{
	shader = new Shader(L"04_World.fxo");

	//dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(2, 5);
	
	D3DXMatrixIdentity(&world);

	vertexCount = (width + 1) * (height + 1);
	vertices = new Vertex[vertexCount];

	//Create Subresource Data(Position)
	{
		for (UINT y = 0; y <= height; y++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * y + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = 0.0f;
				vertices[index].Position.z = (float)y;
			}
		}
	}


	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Create SubresourceData(Index)
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT y = 0; y < height; y++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * y + x; //ÁÂÇÏ0
				indices[index + 1] = (width + 1) * (y + 1) + x; //ÁÂ»ó1
				indices[index + 2] = (width + 1) * y + (x + 1); //¿ìÇÏ2
				indices[index + 3] = (width + 1) * y + (x + 1); //¿ìÇÏ2
				indices[index + 4] = (width + 1) * (y + 1) + x; //ÁÂ»ó1
				indices[index + 5] = (width + 1) * (y + 1) + (x + 1); //ÁÂ»ó1
				
				index += 6;
			}
		}
	}

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}
}

void GirdDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void GirdDemo::Update()
{
	
}


void GirdDemo::Render()
{
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
	shader->AsVector("Color")->SetFloatVector(Color(0, 1, 1, 1));

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;

	shader->DrawIndexed(0, pass, indexCount);
}



