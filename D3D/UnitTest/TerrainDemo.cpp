#include "stdafx.h"
#include "TerrainDemo.h"

void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(110, 50, -110);
	dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"07_Terrain.fxo");
	terrain = new Terrain(shader, L"HeightMap256.png");
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	static UINT pass = shader->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= shader->PassCount();
	terrain->Pass() = pass;
}


void TerrainDemo::Render()
{
	terrain->Render();
}



