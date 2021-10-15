#include "stdafx.h"
#include "MeshDemo.h"
#include "Converter.h"

void MeshDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 1, 0);
	Context::Get()->GetCamera()->Position(0, 17, -30);
	
	shader = new Shader(L"19_Surface.fxo");

	planeMaterial = new Material(shader);
	planeMaterial->DiffuseMap(L"Floor.png");
	
	plane = new MeshRender(shader, new MeshPlane(10, 2));
	plane->AddTransform()->Scale(10, 1, 2);
	plane->UpdateTransforms();

	boxMaterial = new Material(shader);
	boxMaterial->DiffuseMap(L"Box.png");

	box = new MeshRender(shader, new MeshCube());
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = box->AddTransform();
		transform->Scale(0.25f, 0.25f, 0.25f);
		transform->Position(Vector3(x, 0.25f * 0.5f, 0.0f));
		transform->Rotation(0, Math::Random(-(float)D3DX_PI, (float)D3DX_PI), 0);
	}
	box->UpdateTransforms();
}

void MeshDemo::Destroy()
{
	SafeDelete(shader);
	
	SafeDelete(plane);
	SafeDelete(box);

	SafeDelete(planeMaterial);
	SafeDelete(boxMaterial);
}

void MeshDemo::Update()
{
	plane->Update();
	box->Update();
}

void MeshDemo::Render()
{
	planeMaterial->Render();
	plane->Render();

	boxMaterial->Render();
	box->Render();
}
