#include "stdafx.h"
#include "ProjectDemo.h"

void ProjectDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);

	shader = new Shader(L"09_Mesh.fx");

	CreateMesh();

	sDirection = shader->AsVector("LightDirection");

	cubeMapShader = new Shader(L"10_CubeMap.fxo");
	cubeMapMesh = new CubeMapMesh(cubeMapShader);
	cubeMapMesh->Texture(L"Environment/skymap.dds");
	cubeMapMesh->GetTransform()->Position(0, 20, 0);
	cubeMapMesh->GetTransform()->Scale(10, 10, 10);

	sky = new CubeSky(L"Environment/Mountain1024.dds");

	marker = new MeshCylinder(shader, 0.5f, 0.1, 1.0f, 10, 15);
	marker->DiffuseMap(L"Red.png");
	marker->GetTransform()->Position(0, 15, 0);
	marker->GetTransform()->Scale(3, 3, 3);
}

void ProjectDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
	SafeDelete(cube);
	SafeDelete(marker);

	for (UINT i = 0; i < 10; i++)
	{
		SafeDelete(cylinders[i]);
		SafeDelete(spheres[i]);
	}

	SafeDelete(cubeMapShader);
	SafeDelete(cubeMapMesh);
	SafeDelete(sky);
}

void ProjectDemo::Update()
{
	//ImGui Test
	ImGui::SliderFloat3("LightDirection", (float*)&lightDirection, -1, 1);
	sDirection->SetFloatVector(lightDirection);

	static bool bWireframe = false;
	ImGui::Checkbox("Wireframe", &bWireframe);

	static float width = D3D::Width();
	static float height = D3D::Height();
	static float topLeftX = 0;
	static float topLeftY = 0;

	ImGui::SliderFloat("Width", &width, 0, D3D::Width() * 2);
	ImGui::SliderFloat("Height", &height, 0, D3D::Height() * 2);
	ImGui::SliderFloat("X", &topLeftX, -640, +640);
	ImGui::SliderFloat("Y", &topLeftY, -360, +360);

	Viewport* Vp = Context::Get()->GetViewport();
	Vp->Set(width, height, topLeftX, topLeftY);
	//---


	Vector3 P;
	marker->GetTransform()->Position(&P);

	if (Keyboard::Get()->Press(VK_UP))
		P += Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		P -= Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_RIGHT))
		P += Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		P -= Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();

	P.y = 15.0f;
	marker->GetTransform()->Position(P);

	sky->Update();

	quad->Pass(bWireframe ? 1 : 0);
	plane->Pass(bWireframe ? 1 : 0);
	cube->Pass(bWireframe ? 1 : 0);

	static UINT pass = sky->GetShader()->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= sky->GetShader()->PassCount();
	sky->Pass(pass);

	const char* passMode[] = { "Node", "CCW", "Depth Disable" };
	ImGui::LabelText("Mode", "%s", passMode[pass]);


	//Object Update
	quad->Update();
	plane->Update();
	cube->Update();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Pass(bWireframe ? 1 : 0);
		cylinders[i]->Update();

		spheres[i]->Pass(bWireframe ? 1 : 0);
		spheres[i]->Update();
	}

	cubeMapMesh->Update();
	
	marker->Update();
}


void ProjectDemo::Render()
{
	Transform transform;
	transform.World(marker->GetTransform()->World());

	Vector3 temp;
	transform.Position(&temp);
	temp.y += 3;
	transform.Position(temp);

	Matrix W, V, P;
	W = transform.World();
	V = Context::Get()->View();
	P = Context::Get()->Projection();

	Vector3 pPosition = Context::Get()->GetViewport()->Project(Vector3(0, 0, 0), W, V, P);
	Vector3 wPosition = Vector3(transform.World()._41, transform.World()._42, transform.World()._43);

	string str = "P_Position : ";
	str += to_string((int)pPosition.x) + ", " + to_string((int)pPosition.y) + ", " + to_string((int)pPosition.z) + "\n";
	str += "W_Position : ";
	str += to_string((int)wPosition.x) + ", " + to_string((int)wPosition.y) + ", " + to_string((int)wPosition.z);
	
	Gui::Get()->RenderText
	(
		Vector2(pPosition.x, pPosition.y),
		Color(0, 1, 0, 1),
		str
	);

	//sky->Render();

	quad->Render();
	cube->Render();
	plane->Render();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Render();
		spheres[i]->Render();
	}

	cubeMapMesh->Render();

	
	marker->Render();
}

void ProjectDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader, 10.0f, 10.0f);
	plane->GetTransform()->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");

	cube = new MeshCube(shader);
	cube->GetTransform()->Position(0, 5, 0);
	cube->GetTransform()->Scale(20, 10, 20);
	cube->DiffuseMap(L"Stones.png");

	for (UINT i = 0; i < 5; i++)
	{
		cylinders[i * 2 + 0] = new MeshCylinder(shader,0.3f, 0.5f, 3, 1, 20);
		cylinders[i * 2 + 0]->GetTransform()->Position(-30, 6, (float)i * 15.0f - 30.0f);
		cylinders[i * 2 + 0]->GetTransform()->Scale(5, 5, 5);
		cylinders[i * 2 + 0]->DiffuseMap(L"Bricks.png");

		cylinders[i * 2 + 1] = new MeshCylinder(shader, 0.3f, 0.5f, 3, 1, 20);
		cylinders[i * 2 + 1]->GetTransform()->Position(+30, 6, (float)i * 15.0f - 30.0f);
		cylinders[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		cylinders[i * 2 + 1]->DiffuseMap(L"Bricks.png");


		spheres[i * 2 + 0] = new MeshSphere(shader, 0.5f, 20, 20);
		spheres[i * 2 + 0]->GetTransform()->Position(-30, 16, (float)i * 15.0f - 30.0f);
		spheres[i * 2 + 0]->GetTransform()->Scale(5, 5, 5);
		spheres[i * 2 + 0]->DiffuseMap(L"Wall.png");

		spheres[i * 2 + 1] = new MeshSphere(shader, 0.5f, 20, 20);
		spheres[i * 2 + 1]->GetTransform()->Position(+30, 16, (float)i * 15.0f - 30.0f);
		spheres[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		spheres[i * 2 + 1]->DiffuseMap(L"Wall.png");
	}
}



