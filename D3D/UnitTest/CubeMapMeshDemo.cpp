#include "stdafx.h"
#include "CubeMapMeshDemo.h"

void CubeMapMeshDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);

	shader = new Shader(L"09_Mesh.fxo");

	CreateMesh();

	sDirection = shader->AsVector("LightDirection");

	cubeMapShader = new Shader(L"10_CubeMap.fxo");
	cubeMapMesh = new CubeMapMesh(cubeMapShader);
	cubeMapMesh->Texture(L"Environment/skymap.dds");
	cubeMapMesh->Position(0, 20, 0);
	cubeMapMesh->Scale(10, 10, 10);

	sky = new CubeSky(L"Environment/Mountain1024.dds");
}

void CubeMapMeshDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
	SafeDelete(cube);

	for (UINT i = 0; i < 10; i++)
	{
		SafeDelete(cylinders[i]);
		SafeDelete(spheres[i]);
	}

	SafeDelete(cubeMapShader);
	SafeDelete(cubeMapMesh);
	SafeDelete(sky);
}

void CubeMapMeshDemo::Update()
{
	//ImGui Test
	ImGui::SliderFloat3("LightDirection", (float*)&lightDirection, -1, 1);
	sDirection->SetFloatVector(lightDirection);

	static bool bWireframe = false;
	ImGui::Checkbox("Wireframe", &bWireframe);

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
	
}


void CubeMapMeshDemo::Render()
{
	sky->Render();

	quad->Render();
	cube->Render();
	plane->Render();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Render();
		spheres[i]->Render();
	}

	cubeMapMesh->Render();
}

void CubeMapMeshDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader, 10.0f, 10.0f);
	plane->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");

	cube = new MeshCube(shader);
	cube->Position(0, 5, 0);
	cube->Scale(20, 10, 20);
	cube->DiffuseMap(L"Stones.png");

	for (UINT i = 0; i < 5; i++)
	{
		cylinders[i * 2 + 0] = new MeshCylinder(shader,0.3f, 0.5f, 3, 1, 20);
		cylinders[i * 2 + 0]->Position(-30, 6, (float)i * 15.0f - 30.0f);
		cylinders[i * 2 + 0]->Scale(5, 5, 5);
		cylinders[i * 2 + 0]->DiffuseMap(L"Bricks.png");

		cylinders[i * 2 + 1] = new MeshCylinder(shader, 0.3f, 0.5f, 3, 1, 20);
		cylinders[i * 2 + 1]->Position(+30, 6, (float)i * 15.0f - 30.0f);
		cylinders[i * 2 + 1]->Scale(5, 5, 5);
		cylinders[i * 2 + 1]->DiffuseMap(L"Bricks.png");


		spheres[i * 2 + 0] = new MeshSphere(shader, 0.5f, 20, 20);
		spheres[i * 2 + 0]->Position(-30, 16, (float)i * 15.0f - 30.0f);
		spheres[i * 2 + 0]->Scale(5, 5, 5);
		spheres[i * 2 + 0]->DiffuseMap(L"Wall.png");

		spheres[i * 2 + 1] = new MeshSphere(shader, 0.5f, 20, 20);
		spheres[i * 2 + 1]->Position(+30, 16, (float)i * 15.0f - 30.0f);
		spheres[i * 2 + 1]->Scale(5, 5, 5);
		spheres[i * 2 + 1]->DiffuseMap(L"Wall.png");
	}
}



