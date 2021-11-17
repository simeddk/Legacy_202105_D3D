#include "stdafx.h"
#include "TerrainLODDemo.h"

void TerrainLODDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(21, 0, 0);
	Context::Get()->GetCamera()->Position(126, 200, 100);
	((Freedom*)Context::Get()->GetCamera())->Speed(100, 2);

	Perspective* perspective = Context::Get()->GetPerspective();
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 10000, Math::PI * 0.25f);

	shader = new Shader(L"39_TerrainLOD.fxo");
	terrain = new TerrainLOD(L"Terrain/Gray512.dds");
	terrain->BaseMap(L"Terrain/Dirt3.png");
	
}

void TerrainLODDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainLODDemo::Update()
{
	//Height Scale
	ImGui::SliderFloat("HeightScale", &terrain->HeightScale(), 0.001f, 10.0f);

	//MinMax Distance
	ImGui::SliderFloat2("Distance", (float*)&terrain->Distance(), 1, 1000);

	//WireFrame
	static UINT pass = 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass = Math::Clamp<UINT>(pass, 0, 1);
	terrain->Pass(pass);

	terrain->Update();
	
}

void TerrainLODDemo::Render()
{
	terrain->Render();
}
