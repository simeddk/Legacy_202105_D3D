#include "stdafx.h"
#include "Viewer.h"

void Viewer::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	
	particle = new Particle(L"WaterFall");
	shader = new Shader(L"22_AreaLighting.fxo");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	Mesh();
}

void Viewer::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);

	SafeDelete(particle);

	SafeDelete(stone);
	SafeDelete(floor);

	SafeDelete(sphere);
	SafeDelete(grid);
}

void Viewer::Update()
{
	UINT passCount = particle->GetShader()->PassCount();
	ImGui::SliderInt("Pass", (int*)&particle->GetData().Type, 0, passCount - 1);

	static bool bWire = false;
	ImGui::Checkbox("Wire", &bWire);
	grid->Pass(bWire ? 3 : 0);
	sphere->Pass(bWire ? 3 : 0);

	sky->Update();

	grid->Update();
	sphere->Update();

	Vector3 P;
	sphere->GetTransform(0)->Position(&P);
	float moveSpeed = 20.0f;

	if (Mouse::Get()->Press(1) == false)
	{
		const Vector3& F = Context::Get()->GetCamera()->Forward();
		const Vector3& R = Context::Get()->GetCamera()->Right();
		const Vector3& U = Context::Get()->GetCamera()->Up();

		if (Keyboard::Get()->Press('W'))
			P += Vector3(F.x, 0.0f, F.z) * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			P += Vector3(-F.x, 0.0f, -F.z) * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			P += -R * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			P += R * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			P += U * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			P += -U * moveSpeed * Time::Delta();
	}

	sphere->GetTransform(0)->Position(P);
	sphere->UpdateTransforms();

	particle->Emit(P);
	particle->Update();
}

void Viewer::Render()
{
	sky->Render();

	stone->Render(); //MAT
	sphere->Render(); //MESH

	floor->Render(); //MAT
	grid->Render(); //Mesh

	particle->Render();
}

void Viewer::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->SpecularMap("Floor_Specular.png");
		floor->Specular(1, 1, 1, 20);

		stone = new Material(shader);
		stone->DiffuseMap("Bricks.png");
		stone->NormalMap("Bricks_Normal.png");
		stone->SpecularMap("Bricks_Specular.png");
		stone->Specular(1, 1, 1, 20);
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}

	//Create Mesh
	{
		Transform* transform = NULL;

		grid = new MeshRender(shader, new MeshPlane(15, 15));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(20, 1, 20);


		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		transform = sphere->AddTransform();
		transform->Position(0, 5, 0);
		transform->Scale(5, 5, 5);
	}

	sphere->UpdateTransforms();
	grid->UpdateTransforms();
}

void Viewer::Pass(UINT meshPass)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(meshPass);
}
