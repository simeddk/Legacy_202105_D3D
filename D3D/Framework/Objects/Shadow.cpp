#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 & at, float radius, float width, float height)
	: shader(shader)
	, at(at)
	, radius(radius)
	, width(width)
	, height(height)
{
	renderTarget = new RenderTarget(width, height);
	depthStencil = new DepthStencil(width, height);
	viewport = new Viewport(width, height);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");

	sShadowMap = shader->AsSRV("ShadowMap");

	//Create SamplerStaste
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		Check(D3D::GetDevice()->CreateSamplerState(&desc, &sampler));

		sSampler = shader->AsSampler("ShadowSampler");
	}

	desc.MapSize = Vector2(width, height);
}

Shadow::~Shadow()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(buffer);

	SafeRelease(sampler);
}

void Shadow::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Lighting::Get()->Direction();
	Vector3 eye = direction * radius * -2.0f;

	D3DXMatrixLookAtLH(&desc.View, &eye, &at, &up);

	Vector3 center;
	D3DXVec3TransformCoord(&center, &at, &desc.View);

	float l = center.x - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float b = center.y - radius;
	float f = center.z + radius;
	float n = center.z - radius;

	D3DXMatrixOrthoLH(&desc.Projection, r - l, t - b, n, f);

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sShadowMap->SetResource(depthStencil->SRV());

	sSampler->SetSampler(0, sampler);
}
