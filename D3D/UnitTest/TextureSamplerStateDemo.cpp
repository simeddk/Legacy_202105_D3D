#include "stdafx.h"
#include "TextureSamplerStateDemo.h"

void TextureSamplerStateDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 0, -5);
	dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(5, 0);

	shader = new Shader(L"06_TextureSamplerState.fxo");
	texture = new Texture(L"Box.png");

	vertices = new VertexTexture[4];

	vertices[0].Position = Vector3(-0.5f, -0.5f, 0);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0);

	vertices[0].Uv = Vector2(0, 2);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(2, 2);
	vertices[3].Uv = Vector2(2, 0);


	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexTexture) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	indices = new UINT[6]{ 0, 1, 2, 2, 1, 3 };

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * 6;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}

void TextureSamplerStateDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(texture);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void TextureSamplerStateDemo::Update()
{
	if (ImGui::Button("Read Texture"))
	{
		function<void(wstring)> deleOneParam = bind(&TextureSamplerStateDemo::OnReadTexture, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Textures", deleOneParam, D3D::GetDesc().Handle);
	}

	static UINT Filter = 0;
	ImGui::InputInt("Filter", (int*)&Filter);
	Filter %= 2;
	shader->AsScalar("Filter")->SetInt(Filter);

	static UINT Address = 0;
	ImGui::InputInt("Address", (int*)&Address);
	Address %= 4;
	shader->AsScalar("Address")->SetInt(Address);

}


void TextureSamplerStateDemo::Render()
{
	Matrix world;
	D3DXMatrixIdentity(&world);

	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	if (texture != nullptr)
		shader->AsSRV("Texture")->SetResource(texture->SRV());

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT , 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->DrawIndexed(0, 1, 6);

}

void TextureSamplerStateDemo::OnReadTexture(wstring path)
{
	SafeDelete(texture);

	texture = new Texture(path);
}



