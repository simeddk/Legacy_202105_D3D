#include "Framework.h"
#include "Terrain.h"

Terrain::Terrain(Shader * shader, wstring heightMapFile)
	: shader(shader)
{
	heightMap = new Texture(heightMapFile);

	D3DXMatrixIdentity(&world);

	CreateVertexData();
	CreateIndexData();
	CreateNormalData();
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
	static bool bVisibleNormal = false;
	static UINT interval = 3;
	ImGui::Checkbox("Visible Normal", &bVisibleNormal);
	ImGui::SliderInt("Interval", (int*)&interval, 1, 5);

	if (bVisibleNormal)
	{
		for (UINT y = 0; y < height; y += interval)
		{
			for (UINT x = 0; x < width; x += interval)
			{
				UINT index = width * y + x;

				Vector3 start = vertices[index].Position;
				Vector3 end = start + vertices[index].Normal;

				DebugLine::Get()->RenderLine(start, end);
			}
		}
	}

	if (baseMap != nullptr)
		shader->AsSRV("BaseMap")->SetResource(baseMap->SRV());

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

void Terrain::BaseMap(wstring path)
{
	SafeDelete(baseMap);
	baseMap = new Texture(path);
}

float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x  < 0 || x > width - 2) return FLT_MIN;
	if (z  < 0 || z > height - 2) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 p[4];
	for (UINT i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	float u = position.x - p[0].x / 1.0f;
	float v = position.z - p[0].z / 1.0f;

	Vector3 result;

	if (u + v <= 1.0f)
		result = p[0] + (p[2] - p[0]) * u + (p[1] - p[0]) * v;
	else
	{
		u = 1.0f - u;
		v = 1.0f - v;

		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
	}


	return result.y;
}

float Terrain::GetHeight_Raycast(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x  < 0 || x > width - 2) return FLT_MIN;
	if (z  < 0 || z > height - 2) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 p[4];
	for (UINT i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	Vector3 start(position.x, 100.0f, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;

	Vector3 result(-1, FLT_MIN, -1);

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
		result = p[0] + (p[1] - p[0]) * u +  (p[2] - p[0])* v;
	
	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
		result = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;


	return result.y;
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

			vertices[index].Uv.x = x / (float)(width - 1);
			vertices[index].Uv.y = 1.0f - (y / (float)(height - 1));
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

void Terrain::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT vertexIndex0 = indices[i * 3 + 0];
		UINT vertexIndex1 = indices[i * 3 + 1];
		UINT vertexIndex2 = indices[i * 3 + 2];

		VertexTerrain vertex0 = vertices[vertexIndex0];
		VertexTerrain vertex1 = vertices[vertexIndex1];
		VertexTerrain vertex2 = vertices[vertexIndex2];

		Vector3 forward = vertex1.Position - vertex0.Position;
		Vector3 right = vertex2.Position - vertex0.Position;

		Vector3 normal;
		D3DXVec3Cross(&normal, &forward, &right);
		D3DXVec3Normalize(&normal, &normal);

		vertices[vertexIndex0].Normal += normal;
		vertices[vertexIndex1].Normal += normal;
		vertices[vertexIndex2].Normal += normal;
	}

	for (UINT i = 0; i < vertexCount; i++)
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
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
