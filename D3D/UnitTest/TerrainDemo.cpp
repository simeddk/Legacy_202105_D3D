#include "stdafx.h"
#include "TerrainDemo.h"

void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(110, 50, -110);
	dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"08_Terrain.fxo");
	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Sandstone).jpg");
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	static Vector3 LightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("LightDirection", (float*)&LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	static UINT Albedo = 1;
	ImGui::InputInt("Albedo", (int*)&Albedo);
	Albedo %= 2;
	shader->AsScalar("Albedo")->SetInt(Albedo);
}


void TerrainDemo::Render()
{
	terrain->Render();
}



