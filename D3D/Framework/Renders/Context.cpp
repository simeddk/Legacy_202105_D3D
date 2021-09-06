#include "Framework.h"
#include "Context.h"
#include "Viewer/Viewport.h"
#include "Viewer/Perspective.h"

Context* Context::instance = NULL;

Context * Context::Get()
{
	//assert(instance != NULL);

	return instance;
}

void Context::Create()
{
	assert(instance == NULL);

	instance = new Context();
}

void Context::Delete()
{
	SafeDelete(instance);
}

Context::Context()
{
	D3DDesc desc = D3D::GetDesc();

	perspective = new Perspective(desc.Width, desc.Height);
	viewport = new Viewport(desc.Width, desc.Height);


	position = D3DXVECTOR3(0, 0, -10);
	D3DXVECTOR3 forward(0, 0, 1);
	D3DXVECTOR3 right(1, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);

	D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);
}

Context::~Context()
{
	SafeDelete(perspective);
	SafeDelete(viewport);
}

void Context::ResizeScreen()
{
	perspective->Set(D3D::Width(), D3D::Height());
	viewport->Set(D3D::Width(), D3D::Height());
}

void Context::Update()
{
	
}

void Context::Render()
{
	viewport->RSSetViewport();
}

D3DXMATRIX Context::Projection()
{
	D3DXMATRIX projection;
	perspective->GetMatrix(&projection);

	return projection;
}
