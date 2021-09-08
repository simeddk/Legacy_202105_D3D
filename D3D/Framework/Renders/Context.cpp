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

	camera = new Freedom();
}

Context::~Context()
{
	SafeDelete(perspective);
	SafeDelete(viewport);
	SafeDelete(camera);
}

void Context::ResizeScreen()
{
	perspective->Set(D3D::Width(), D3D::Height());
	viewport->Set(D3D::Width(), D3D::Height());
}

void Context::Update()
{
	camera->Update();
}

void Context::Render()
{
	viewport->RSSetViewport();

	//FPS
	string str = string("FPS : ") + to_string(ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(Vector2(5, 5), Color(1, 1, 1, 1), str);

	//Camera Information
	Vector3 position;
	camera->Position(&position);

	Vector3 rotation;
	camera->RotationDegree(&rotation);

	str = "Camera(P) : ";
	str += to_string(position.x);
	str += ", ";
	str += to_string(position.y);
	str += ", ";
	str += to_string(position.z);

	Gui::Get()->RenderText(Vector2(5, 20), Color(1, 1, 1, 1), str);

	str = "Camera(R) : ";
	str += to_string(rotation.x);
	str += ", ";
	str += to_string(rotation.y);

	Gui::Get()->RenderText(Vector2(5, 35), Color(1, 1, 1, 1), str);
}

D3DXMATRIX Context::View()
{
	Matrix view;
	camera->GetMatrix(&view);

	return view;
}

D3DXMATRIX Context::Projection()
{
	D3DXMATRIX projection;
	perspective->GetMatrix(&projection);

	return projection;
}
