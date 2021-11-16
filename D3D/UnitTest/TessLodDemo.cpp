#include "stdafx.h"
#include "TessLodDemo.h"

void TessLodDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(0, 0, 0);
	Context::Get()->GetCamera()->Position(0, 0, -5);
	((Freedom*)Context::Get()->GetCamera())->Speed(10, 0);

	shader = new Shader(L"38_Lod.fxo");
	perFrame = new PerFrame(shader);

	Vertex vertices[4];
	vertices[0].Position = Vector3(-0.9f, -0.9f, 0.0f);
	vertices[1].Position = Vector3(-0.9f, +0.9f, 0.0f);
	vertices[2].Position = Vector3(+0.9f, -0.9f, 0.0f);
	vertices[3].Position = Vector3(+0.9f, +0.9f, 0.0f);

	vertexBuffer = new VertexBuffer(vertices, 4, sizeof(Vertex));
}

void TessLodDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(vertexBuffer);
	SafeDelete(perFrame);
}

void TessLodDemo::Update()
{
	static Vector2 distance = Vector2(1, 100);
	ImGui::SliderFloat2("Distance", distance, 0, 1000);
	
	shader->AsVector("Distance")->SetFloatVector(distance);
	
	perFrame->Update();


}

void TessLodDemo::Render()
{
	perFrame->Render();

	vertexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	shader->Draw(0, 0, 4);
}
