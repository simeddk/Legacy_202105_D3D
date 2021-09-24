#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 1, 0);
	Context::Get()->GetCamera()->Position(0, 17, -30);

	shader = new Shader(L"11_Model.fxo");

	Tank();
	Kachujin();

	sky = new CubeSky(L"Environment/Mountain1024.dds");
	sky->Pass(2);
	planeShader = new Shader(L"09_Mesh.fxo");

	plane = new MeshPlane(planeShader, 6, 6);
	plane->GetTransform()->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");
}

void ModelDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tank);
	SafeDelete(kachujin);

	SafeDelete(sky);
	SafeDelete(planeShader);
	SafeDelete(plane);
}

void ModelDemo::Update()
{
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	planeShader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;
	tank->Pass(pass);

	sky->Update();
	plane->Update();

	if (tank != nullptr)
		tank->Update();

	if (kachujin != nullptr)
		kachujin->Update();
}

void ModelDemo::Render()
{
	sky->Render();
	plane->Render();

	if (tank != nullptr)
		tank->Render();

	if (kachujin != nullptr)
		kachujin->Render();
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
}

void ModelDemo::Kachujin()
{
	kachujin = new ModelRender(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");

	kachujin->GetTransform()->Position(5, 0, 0);
	kachujin->GetTransform()->Scale(0.01f, 0.01f, 0.01f);
}

