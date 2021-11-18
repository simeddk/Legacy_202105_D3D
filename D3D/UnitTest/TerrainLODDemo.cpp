#include "stdafx.h"
#include "TerrainLODDemo.h"

void TerrainLODDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(21, 0, 0);
	Context::Get()->GetCamera()->Position(126, 200, 100);
	((Freedom*)Context::Get()->GetCamera())->Speed(1000, 2);

	Perspective* perspective = Context::Get()->GetPerspective();
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 10000, Math::PI * 0.25f);

	shader = new Shader(L"39_TerrainLOD.fxo");
	terrain = new TerrainLOD(L"Terrain/Gray1920.dds");

	terrain->BaseMap(L"Terrain/Cliff (Layered Rock).jpg");
	terrain->NormalMap(L"Terrain/Cliff (Layered Rock)_NormalMap.png");
	
}

void TerrainLODDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainLODDemo::Update()
{
	//Lgiht
	ImGui::SliderFloat3("Light", Lighting::Get()->Direction(), -1, 1);

	//Height Scale
	ImGui::SliderFloat("HeightScale", &terrain->HeightScale(), 0.001f, 10.0f);

	//MinMax Distance
	ImGui::SliderFloat2("Distance", (float*)&terrain->Distance(), 1, 1000);

	terrain->Update();
	
}

void TerrainLODDemo::Render()
{
	terrain->Render();
}
