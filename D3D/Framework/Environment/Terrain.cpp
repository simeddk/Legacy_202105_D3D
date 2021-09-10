#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Shader * shader, wstring heightMapFile)
	: shader(shader)
{
	heightMap = new Texture(heightMapFile);

	D3DXMatrixIdentity(&world);

	CreateVertexData();
	CreateIndexData();
	CreateBuffer();
}

Terrain::~Terrain()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(heightMap);
}

void Terrain::Update()
{
}

void Terrain::Render()
{
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	UINT stride = sizeof(VertexTerrain);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->DrawIndexed(0, pass, indexCount);
}

void Terrain::CreateVertexData()
{
	width = heightMap->GetWidth();
	height = heightMap->GetHeight();
	
	vector<Color> colors;
	heightMap->ReadPixel(&colors);
	
	vertexCount = width * height;
	vertices = new VertexTerrain[vertexCount];

	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * y + x;
			UINT reverse = width * (height - y - 1) + x;

			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = colors[reverse].r * 255.0f / 10.0f;
			vertices[index].Position.z = (float)y;
		}
	}
}

void Terrain::CreateIndexData()
{
	indexCount = (width - 1) * (height - 1) * 6;
	indices = new UINT[indexCount];

	UINT index = 0;
	for (UINT y = 0; y < height - 1; y++)
	{
		for (UINT x = 0; x < width - 1; x++)
		{
			indices[index + 0] = (width) * y + x; //ÁÂÇÏ0
			indices[index + 1] = (width) * (y + 1) + x; //ÁÂ»ó1
			indices[index + 2] = (width) * y + (x + 1); //¿ìÇÏ2
			indices[index + 3] = (width) * y + (x + 1); //¿ìÇÏ2
			indices[index + 4] = (width) * (y + 1) + x; //ÁÂ»ó1
			indices[index + 5] = (width) * (y + 1) + (x + 1); //ÁÂ»ó1

			index += 6;
		}
	}
}

void Terrain::CreateBuffer()
{
	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexTerrain) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
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
