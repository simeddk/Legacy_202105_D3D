#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Shader * shader)
	: shader(shader)
{
	D3DXMatrixIdentity(&world);

	sWorld = shader->AsMatrix("World");
	sView = shader->AsMatrix("View");
	sProjection = shader->AsMatrix("Projection");

	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(diffuseMap);
}

void Mesh::Update()
{
}

void Mesh::Render()
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
	{
		Create();
		CreateBuffer();
	}

	sWorld->SetMatrix(world);
	sView->SetMatrix(Context::Get()->View());
	sProjection->SetMatrix(Context::Get()->Projection());

	if (diffuseMap != nullptr)
	{
		sDiffuseMap->SetResource(diffuseMap->SRV());
	}

	UINT stride = sizeof(VertexMesh);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexed(0, pass, indexCount);
}

void Mesh::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Mesh::Position(Vector3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Mesh::Position(Vector3 * vec)
{
	*vec = position;
}

void Mesh::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Mesh::Rotation(Vector3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void Mesh::Rotation(Vector3 * vec)
{
	*vec = rotation;
}

void Mesh::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Mesh::RotationDegree(Vector3 & vec)
{
	rotation = vec * Math::PI / 180.0f;

	UpdateWorld();
}

void Mesh::RotationDegree(Vector3 * vec)
{
	*vec = rotation * 180.0f / Math::PI;
}

void Mesh::Scale(float x, float y, float z)
{
	Scale(Vector3(x, y, z));
}

void Mesh::Scale(Vector3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Mesh::Scale(Vector3 * vec)
{
	*vec = scale;
}

Vector3 Mesh::Foward()
{
	return Vector3(world._31, world._32, world._33);
}

Vector3 Mesh::Up()
{
	return Vector3(world._21, world._22, world._23);
}

Vector3 Mesh::Right()
{
	return Vector3(world._11, world._12, world._13);
}

void Mesh::DiffuseMap(wstring file)
{
	SafeDelete(diffuseMap);

	diffuseMap = new Texture(file);
}

void Mesh::CreateBuffer()
{
	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexMesh) * vertexCount;
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

void Mesh::UpdateWorld()
{
	Matrix S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;
}
