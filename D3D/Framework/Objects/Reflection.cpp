#include "Framework.h"
#include "Reflection.h"

Reflection::Reflection(Shader * shader, Transform * transform, float width, float height)
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
	SafeDelete(camera);
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
}

void Reflection::Update()
{
	Vector3 R, T;
	Context::Get()->GetCamera()->Rotation(&R);
	Context::Get()->GetCamera()->Position(&T);

	R.x *= -1.0f;
	camera->Rotation(R);

	Vector3 position;
	transform->Position(&position);

	T.y = (position.y * 2.0f) - T.y;
	camera->Position(T);

}

void Reflection::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Matrix view;
	camera->GetMatrix(&view);
	sReflectionView->SetMatrix(view);

	Vector3 position;
	transform->Position(&position);
	Plane plane = Plane(0, 1, 0, -position.y);
	Context::Get()->Clipping() = plane;
}

void Reflection::Render()
{
	sReflectionSRV->SetResource(renderTarget->SRV());
}
