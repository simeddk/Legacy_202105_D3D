#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 1, 0);
	Context::Get()->GetCamera()->Position(0, 17, -30);
	shader = new Shader(L"19_Surface.fxo");

	Tank();
	Tower();
	Airplane();

	
}

void ModelDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tank);
	SafeDelete(airplane);
	SafeDelete(tower);
	
}

void ModelDemo::Update()
{
	//Directional Light
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	//instance Rotation(Tower)
	static UINT index = 0;
	ImGui::SliderInt("InstanceIndex", (int*)&index, 0, tower->TransformCount() - 1);

	Transform* transform = tower->GetTransform(index);
	Vector3 R;
	transform->RotationDegree(&R);
	ImGui::SliderFloat3("InstanceRotation", (float*)&R, -179.9f, 179.9f);
	transform->RotationDegree(R);
	tower->UpdateTransforms();

	//Instance Color(Airplane)
	airplane->SetColor(9, Color(1, 0, 0, 1));
	airplane->SetColor(10, Color(0, 1, 0, 1));
	tank->SetColor(index, Color(0, 0, 1, 0));
	airplane->UpdateTransforms();
	tank->UpdateTransforms();

	
	if (tank != nullptr)
	{
		tank->Update();
	}

	if (tower != nullptr)
	{
		tower->Update();
	}

	if (airplane != nullptr)
	{
		airplane->Update();
	}
}

void ModelDemo::Render()
{
	
	if (tank != nullptr)
	{
		tank->Pass(1);
		tank->Render();
	}

	if (tower != nullptr)
	{
		tower->Pass(1);
		tower->Render();
	}
	
	if (airplane != nullptr)
	{
		airplane->Pass(1);
		airplane->Render();
	}
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
	tank->ReadMaterial(L"Tank/Tank");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform();
		transform->Scale(0.1f, 0.1f, 0.1f);
		transform->Position(x, 0, 5);
		transform->RotationDegree(0.0f, Math::Random(-180.0f, 180.0f), 0.0f);
	}
	tank->UpdateTransforms();
}

void ModelDemo::Tower()
{
	tower = new ModelRender(shader);
	tower->ReadMesh(L"Tower/Tower");
	tower->ReadMaterial(L"Tower/Tower");

	//tower->GetTransform()->Position(-5, 0, 0);
	//tower->GetTransform()->Scale(0.01f, 0.01f, 0.01f);

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tower->AddTransform();
		transform->Scale(0.003f, 0.003f, 0.003f);
		transform->Position(x, 0, 7.5f);
		transform->RotationDegree(0.0f, Math::Random(-180.0f, 180.0f), 0.0f);
	}
	tower->UpdateTransforms();
}

void ModelDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	//airplane->GetTransform()->Position(-10, 0, 0);
	//airplane->GetTransform()->Scale(0.001f, 0.001f, 0.001f);

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = airplane->AddTransform();
		transform->Scale(0.00025f, 0.00025f, 0.00025f);
		transform->Position(x, 0, 2.5f);
		transform->RotationDegree(0.0f, Math::Random(-180.0f, 180.0f), 0);
	}
	airplane->UpdateTransforms();
}

