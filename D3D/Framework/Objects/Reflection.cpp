#include "Framework.h"
#include "Reflection.h"

Reflection::Reflection(Shader * shader, Transform * transform, wstring normalFile, float width, float height)
	: shader(shader)
	, transform(transform)
{
	this->width = width > 0.0f ? width : (UINT)D3D::Width();
	this->height = height > 0.0f ? height : (UINT)D3D::Height();


	camera = new Fixity();
	renderTarget = new RenderTarget(this->width, this->height);
	depthStencil = new DepthStencil(this->width, this->height);
	viewport = new Viewport(this->width, this->height);

	sReflectionSRV = shader->AsSRV("ReflectionMap");
	sReflectionView = shader->AsMatrix("ReflectionView");
}

Reflection::~Reflection()
{
}

void Reflection::Update()
{
}

void Reflection::PreRender()
{
}

void Reflection::Render()
{
}
