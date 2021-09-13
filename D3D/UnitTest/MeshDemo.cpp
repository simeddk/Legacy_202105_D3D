#include "stdafx.h"
#include "MeshDemo.h"

void MeshDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);

	shader = new Shader(L"09_Mesh.fxo");

	CreateMesh();

	sDirection = shader->AsVector("LightDirection");
}

void MeshDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
}

void MeshDemo::Update()
{
	ImGui::SliderFloat3("LightDirection", (float*)&lightDirection, -1, 1);
	sDirection->SetFloatVector(lightDirection);

	quad->Update();
	plane->Update();
}


void MeshDemo::Render()
{
	//quad->Pass(1);
	//plane->Pass(1);

	quad->Render();
	plane->Render();
}

void MeshDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader, 2.5f, 2.5f);
	plane->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");
}



