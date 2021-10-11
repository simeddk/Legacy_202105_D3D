#include "Framework.h"
#include "Render2D.h"

Render2D::Render2D()
	: Renderer(L"15_Render2D.fxo")
{
	D3DXMatrixLookAtLH(&desc.View, &Vector3(0, 0, -1), &Vector3(0, 0, 0), &Vector3(0, 1, 0));
	D3DXMatrixOrthoOffCenterLH(&desc.Projection, 0, D3D::Width(), 0, D3D::Height(), -1, +1);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	shader->AsConstantBuffer("CB_Render2D")->SetConstantBuffer(buffer->Buffer());

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, +0.5f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 4, sizeof(VertexTexture));

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };
	indexBuffer = new IndexBuffer(indices, 6);

	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Render2D::~Render2D()
{
	SafeDelete(buffer);
}

void Render2D::Update()
{
	Super::Update();
}

void Render2D::Render()
{
	Super::Render();

	buffer->Render();
	shader->DrawIndexed(0, 0, 6);
}

void Render2D::SRV(ID3D11ShaderResourceView * srv)
{
	sDiffuseMap->SetResource(srv);
}
