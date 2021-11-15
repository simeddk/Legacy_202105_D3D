#include "stdafx.h"
#include "Brush.h"

Brush::Brush(Shader * shader, Terrain * terrain)
	: shader(shader)
	, terrain(terrain)
{
	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_TerrainBrush");

	lineBuffer = new ConstantBuffer(&lineDesc, sizeof(LineDesc));
	sLineBuffer = shader->AsConstantBuffer("CB_TerrainLine");
}

Brush::~Brush()
{
	SafeDelete(brushBuffer);
	SafeDelete(lineBuffer);
}

void Brush::Update()
{
	ImGui::Separator();

	static bool bVisible = true;
	ImGui::Checkbox("LineVisible", &bVisible);
	lineDesc.Visible = bVisible ? 1 : 0;

	ImGui::ColorEdit3("LineColor", lineDesc.Color);
	ImGui::InputFloat("Thickness", &lineDesc.Thickness, 0.01f);
	ImGui::InputFloat("LineSize", &lineDesc.Size, 1.0f);

	ImGui::Separator();

	ImGui::ColorEdit3("BrushColor", brushDesc.Color);
	ImGui::InputInt("BrushType", (int*)&brushDesc.Type);

	ImGui::InputInt("BrushRange", (int*)&brushDesc.Range);
	brushDesc.Range = Math::Clamp<UINT>(brushDesc.Range, 1, 10);

	if (brushDesc.Type > 0)
	{
		brushDesc.Location = terrain->GetMousePosition();

		if (brushDesc.Type == 1)
		{
			if (Mouse::Get()->Press(0))
				RaiseHeight(5);
		}
	}
}

void Brush::Render()
{
	if (brushDesc.Type > 0)
	{
		string str = "";
		str += to_string(brushDesc.Location.x);
		str += ", ";
		str += to_string(brushDesc.Location.y);
		str += ", ";
		str += to_string(brushDesc.Location.z);

		Gui::Get()->RenderText(10, 50, 1, 0, 0, str);
	}

	brushBuffer->Render();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());

	lineBuffer->Render();
	sLineBuffer->SetConstantBuffer(lineBuffer->Buffer());
}

void Brush::RaiseHeight(float intensity)
{
	UINT left = (UINT)brushDesc.Location.x - brushDesc.Range;
	UINT right = (UINT)brushDesc.Location.x + brushDesc.Range;
	UINT bottom = (UINT)brushDesc.Location.z - brushDesc.Range;
	UINT top = (UINT)brushDesc.Location.z + brushDesc.Range;

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = terrain->Width() * z + x;

			terrain->Vertices()[index].Position.y += intensity * Time::Delta();
		}
	}

	terrain->UpdateVertexData();
	terrain->CreateNormalData();
}
