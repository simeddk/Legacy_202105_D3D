#include "stdafx.h"
#include "UnprojectDemo.h"

void UnprojectDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(110, 50, -110);
	dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"08_Terrain.fxo");
	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Sandstone).jpg");
}

void UnprojectDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void UnprojectDemo::Update()
{
	static Vector3 LightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("LightDirection", (float*)&LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	static UINT Albedo = 1;
	ImGui::InputInt("Albedo", (int*)&Albedo);
	Albedo %= 2;
	shader->AsScalar("Albedo")->SetInt(Albedo);
	
	terrain->Update();
}


void UnprojectDemo::Render()
{
	Vector3 position = terrain->GetMousePosition();
	string str;
	str += to_string(position.x) + ", ";
	str += to_string(position.y) + ", ";
	str += to_string(position.z);
	Gui::Get()->RenderText(Vector2(10, 70), Color(1, 0, 0, 1), str);


	terrain->Render();
}



