#include "stdafx.h"
#include "ScatteringDemo.h"

void ScatteringDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(126, 36, 39);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"41_Scattering.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 65);

	sky = new CubeSky(L"Environment/Mountain1024.dds", shader);

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Layered Rock).jpg");
	terrain->NormalMap(L"Terrain/Cliff (Layered Rock)_NormalMap.png");

	render2D = new Render2D();
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->GetTransform()->Scale(320, 180, 1);
	render2D->SRV(shadow->SRV());

	Mesh();
	Airplane();
	Kachujin();
	Weapon();

	PointLights();
	SpotLights();

}

void ScatteringDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);
	SafeDelete(terrain);
	
	//Material
	SafeDelete(floor);
	SafeDelete(stone);
	SafeDelete(brick);
	SafeDelete(wall);
	
	//Mesh
	SafeDelete(cube);
	SafeDelete(plane);
	SafeDelete(cylinder);
	SafeDelete(sphere);

	//Model
	SafeDelete(airplane);
	SafeDelete(kachujin);
	SafeDelete(weapon);

	//Shadow
	SafeDelete(shadow);
	SafeDelete(render2D);
}

void ScatteringDemo::Update()
{
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, 1);
	
	static UINT q = 0;
	ImGui::InputInt("Quality", (int*)&q);
	q %= 3;
	shadow->Quality(q);

	static float bias = 0;
	ImGui::SliderFloat("Bias", &bias, -0.2f, 0.2f);
	shadow->Bias(bias);

	sky->Update();

	cube->Update();
	plane->Update();
	cylinder->Update();
	sphere->Update();

	airplane->Update();
	kachujin->Update();

	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < kachujin->TransformCount(); i++)
	{
		kachujin->GetAttachBones(i, worlds);
		weapon->GetTransform(i)->World(weaponTransform->World() * worlds[40]);
	}
	
	weapon->UpdateTransforms();
	weapon->Update();

	render2D->Update();

	terrain->Update();
}

void ScatteringDemo::PreRender()
{
	shadow->PreRender();
	{
		Pass(0);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();
		
		airplane->Render();
		kachujin->Render();
		weapon->Render();

		terrain->Pass(3);
		terrain->Render();
	}
}

void ScatteringDemo::Render()
{ 
	sky->Pass(3);
	sky->Render();

	Pass(4);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();
	plane->Render();

	airplane->Render();
	kachujin->Render();
	weapon->Render();
	
	render2D->Render();

	terrain->Pass(7);
	terrain->Render();
}

void ScatteringDemo::PostRender()
{
}

void ScatteringDemo::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->Specular(1, 1, 1, 20);
		floor->NormalMap("Floor_Normal.png");
		floor->SpecularMap("Floor_Specular.png");

		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->Specular(1, 1, 1, 20);
		stone->NormalMap("Stones_Normal.png");
		stone->SpecularMap("Stones_Specular.png");
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 1, 1, 20);
		brick->NormalMap("Bricks_Normal.png");
		brick->SpecularMap("Bricks_Specular.png");
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->Specular(1, 1, 1, 20);
		wall->NormalMap("Wall_Normal.png");
		wall->SpecularMap("Wall_Specular.png");
		wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}

	//Create Mesh
	{
		Transform* transform = nullptr;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(128, 5, 128);
		transform->Scale(20, 10, 20);
		SetTransform(transform);

		plane = new MeshRender(shader, new MeshPlane(2.5f, 2.5f));
		transform = plane->AddTransform();
		transform->Scale(12, 1, 12);

		cylinder = new MeshRender(shader, new MeshCylinder(0.3f, 0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0 ; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(128 - 30, 6, 128 + (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);

			transform = cylinder->AddTransform();
			transform->Position(128 + 30, 6, 128 + (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);

			transform = sphere->AddTransform();
			transform->Position(128 -30, 15.5f, 128 + (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);

			transform = sphere->AddTransform();
			transform->Position(128 + 30, 15.5f, 128 + (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);
			SetTransform(transform);
		}

		
	}

	cube->UpdateTransforms();
	plane->UpdateTransforms();
	cylinder->UpdateTransforms();
	sphere->UpdateTransforms();

	meshes.push_back(cube);
	meshes.push_back(plane);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
}

void ScatteringDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane"); //TODO. 노멀맵 머티리얼 변경하기
	
	Transform* transform = airplane->AddTransform();
	transform->Scale(0.004f, 0.004f, 0.004f);
	transform->Position(128 + 2.0f, 9.91f, 128 + 2.0f);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void ScatteringDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");

	Transform* transform = nullptr;
	
	transform = kachujin->AddTransform();
	transform->Position(128 + 0, 0, 128 -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	kachujin->PlayTweenMode(0, 0, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(128 -15, 0, 128 -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	kachujin->PlayTweenMode(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(128 -30, 0, 128 -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	kachujin->PlayTweenMode(2, 2, 0.75f);

	transform = kachujin->AddTransform();
	transform->Position(128 + 15, 0, 128 -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	kachujin->PlayBlendMode(3, 0, 1, 2);
	kachujin->SetBlendAlpha(3, 1.75f);

	transform = kachujin->AddTransform();
	transform->Position(128 + 30, 0, 128 -32.5f);
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform);
	kachujin->PlayTweenMode(4, 4, 0.75f);

	kachujin->UpdateTransforms();

	animators.push_back(kachujin);
}


void ScatteringDemo::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	UINT count = kachujin->TransformCount();
	for (UINT i = 0; i < count; i++)
		weapon->AddTransform();

	weapon->UpdateTransforms();
	models.push_back(weapon);


	weaponTransform = new Transform();
	weaponTransform->Position(-2.9f, 1.45f, -6.45f);
	weaponTransform->Scale(0.5f, 0.5f, 0.75f);
	weaponTransform->Rotation(0, 0, 1);
}

void ScatteringDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient;
		Color(0.0f, 0.3f, 1.0f, 1.0f),//Diffuse;
		Color(0.0f, 0.0f, 0.7f, 1.0f),//Specular;
		Color(0.0f, 0.0f, 0.7f, 1.0f),//Emissive;
		Vector3(128 -30, 10, 128 -30),//Position;
		15.0f, //Range;
		0.9f //Intensity;
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient;
		Color(1.0f, 0.0f, 0.0f, 1.0f),//Diffuse;
		Color(0.6f, 0.2f, 0.0f, 1.0f),//Specular;
		Color(0.6f, 0.3f, 0.0f, 1.0f),//Emissive;
		Vector3(128 + 15, 10, 128 -30),//Position;
		10.0f, //Range;
		1.0f //Intensity;
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient;
		Color(0.0f, 1.0f, 0.0f, 1.0f),//Diffuse;
		Color(0.0f, 0.7f, 0.0f, 1.0f),//Specular;
		Color(0.0f, 0.7f, 0.0f, 1.0f),//Emissive;
		Vector3(128 -5, 1, 128 -17.0f),//Position;
		5.0f, //Range;
		0.9f //Intensity;
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient;
		Color(0.0f, 0.0f, 1.0f, 1.0f),//Diffuse;
		Color(0.0f, 0.0f, 0.7f, 1.0f),//Specular;
		Color(0.0f, 0.0f, 0.7f, 1.0f),//Emissive;
		Vector3(128 -10, 1, 128 -17.0f),//Position;
		5.0f, //Range;
		0.9f //Intensity;
	};
	Lighting::Get()->AddPointLight(light);
}

void ScatteringDemo::SpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f), //Ambient
		Color(0.7f, 1.0f, 0.0f, 1.0f), //Diffuse
		Color(0.3f, 1.0f, 0.0f, 1.0f), //Specular
		Color(0.3f, 1.0f, 0.0f, 1.0f), //Emissive
		Vector3(128 -15, 20, 128 -30), //Position
		25.0f, //Range
		Vector3(0, -1, 0), //Direction
		30.0f, //Angle
		0.9f //Intensity
	};
	Lighting::Get()->AddSpotLight(light);


	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f), //Ambient
		Color(1.0f, 0.2f, 0.9f, 1.0f), //Diffuse
		Color(1.0f, 0.2f, 0.9f, 1.0f), //Specular
		Color(1.0f, 0.2f, 0.9f, 1.0f), //Emissive
		Vector3(128 + 0, 20, 128 - 30), //Position
		30.0f, //Range
		Vector3(0, -1, 0), //Direction
		40.0f, //Angle
		0.9f //Intensity
	};
	Lighting::Get()->AddSpotLight(light);



}


void ScatteringDemo::Pass(UINT val)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(val);

	val++;
	for (ModelRender* model : models)
		model->Pass(val);

	val++;
	for (ModelAnimator* animator : animators)
		animator->Pass(val);
}

void ScatteringDemo::SetTransform(Transform * transform)
{
	Vector3 position;
	transform->Position(&position);

	Vector3 scale;
	transform->Scale(&scale);

	float y = terrain->GetHeight(position);
	position.y += y + scale.y * 0.5f;

	transform->Position(position);
}
