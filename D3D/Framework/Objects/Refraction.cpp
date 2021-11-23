#include "Framework.h"
#include "Refraction.h"

Refraction::Refraction(Shader * shader, Transform * transform, wstring normalFile, float width, float height)
	: shader(shader)
	, transform(transform)
{
	this->width = width > 0.0f ? width : D3D::Width();
	this->height = height > 0.0f ? height : D3D::Height();

	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport(this->width, this->height);

	sRefractionSRV = shader->AsSRV("RefractionMap");

	normalMap = new Texture(normalFile);
	sNormalMap = shader->AsSRV("NormalMap");

	OnButtonPressed_NormalMap = bind(&Refraction::NormalMap, this, placeholders::_1);
}

Refraction::~Refraction()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(normalMap);
}

void Refraction::Update()
{
}

void Refraction::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Vector3 position;
	transform->Position(&position);
	Plane plane = Plane(0, -1, 0, position.y);
	Context::Get()->Clipping() = plane;

}

void Refraction::Render()
{
	sRefractionSRV->SetResource(renderTarget->SRV());
	sNormalMap->SetResource(normalMap->SRV());
}

void Refraction::NormalMap(wstring normalFile)
{
	SafeDelete(normalMap);
	normalMap = new Texture(normalFile);

}
