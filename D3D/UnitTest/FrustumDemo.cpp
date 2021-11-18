#include "stdafx.h"
#include "FrustumDemo.h"


void FrustumDemo::Initialize()
{
	gridShader = new Shader(L"40_Frustum.fxo");

	floor = new Material(gridShader);
	floor->DiffuseMap(L"White.png");

	plane = new MeshRender(gridShader, new MeshPlane());
	plane->AddTransform()->Scale(10, 10, 10);
	plane->UpdateTransforms();
	plane->Pass(0);

}

void FrustumDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(collider[0]);
	SafeDelete(collider[1]);
	SafeDelete(transform[0]);
	SafeDelete(transform[0]);
	
}

void FrustumDemo::Update()
{
	//Collider[0]
	Vector3 position;
	transform[0]->Position(&position);

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 3.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_UP))
		position.z += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 3.0f * Time::Delta();

	transform[0]->Position(position);

	//Collider[1]
	Vector3 position2;
	transform[1]->Position(&position2);

	if (Keyboard::Get()->Press(VK_NUMPAD6))
		position2.x += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_NUMPAD4))
		position2.x -= 3.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_NUMPAD8))
		position2.z += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_NUMPAD5))
		position2.z -= 3.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_NUMPAD7))
		position2.y -= 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_NUMPAD9))
		position2.y += 3.0f * Time::Delta();

	transform[1]->Position(position2);


	//----
	static Vector3 rotation = Vector3(0, 0, 0);
	static Vector3 rotation2 = Vector3(0, 0, 0);

	ImGui::SliderFloat3("Rotation", (float*)&rotation, -179, 179);
	ImGui::SliderFloat3("Rotation2", (float*)&rotation2, -179, 179);

	transform[0]->RotationDegree(rotation);
	transform[1]->RotationDegree(rotation2);


	collider[0]->Update();
	collider[1]->Update();
}

void FrustumDemo::Render()
{
	bool b = collider[0]->Intersection(collider[1]);
	Color color = b ? Color(1, 0, 0, 1) : Color(0, 1, 0, 1);

	collider[0]->Render(color);
	collider[1]->Render(color);
}
