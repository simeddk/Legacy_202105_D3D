#include "stdafx.h"
#include "ReflectionDemo.h"

void ReflectionDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(126, 36, 39);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"43_Reflection.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 128);

	sky = new Sky(shader);

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Layered Rock).jpg");
	terrain->NormalMap(L"Terrain/Cliff (Layered Rock)_NormalMap.png");


	Mesh();
	Airplane();
	Kachujin();
	Weapon();

	PointLights();
	SpotLights();

	reflection = new Reflection(shader, mirrorPlane->GetTransform(0));

}

void ReflectionDemo::Destroy()
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
	SafeDelete(cylinder);
	SafeDelete(sphere);

	//Model
	SafeDelete(airplane);
	SafeDelete(kachujin);
	SafeDelete(weapon);

	//Shadow
	SafeDelete(shadow);

	//Water
	SafeDelete(mirrorPlane);
	SafeDelete(reflection);
}

void ReflectionDemo::Update()
{
	//ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, 1);
	
	//Scattering Test
	{
		static bool bRealTime = false;
		static float speed = 1.0f;
		ImGui::Checkbox("RealTime", &bRealTime);
		ImGui::SliderFloat("Speed", &speed, 0.5f, 3.0f);
		sky->RealTime(bRealTime, speed);
	}

	//Fog Test
	{
		ImGui::Separator();
		static UINT type = Lighting::Get()->FogType();
		ImGui::InputInt("Fog Type", (int*)&type);
		type %= 3;
		Lighting::Get()->FogType() = type;

		ImGui::ColorEdit3("Fog Color", Lighting::Get()->FogColor());
		ImGui::SliderFloat2("Fog Distance", Lighting::Get()->FogDistance(), 1, 200);
		ImGui::SliderFloat("Fog Density", &Lighting::Get()->FogDensity(), 0, 5);
		ImGui::Separator();
	}

	
	sky->Update();

	cube->Update();
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

	terrain->Update();

	reflection->Update();
	mirrorPlane->Update();
}

void ReflectionDemo::PreRender()
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

	//Reflection
	reflection->PreRender();
	{
		sky->Pass(15);
		sky->Render();

		Pass(11);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();

		airplane->Render();
		kachujin->Render();
		weapon->Render();

		terrain->Pass(14);
		terrain->Render();
	}

	


	sky->PreRender();
}

void ReflectionDemo::Render()
{ 
	sky->Pass(8);
	sky->Render();

	Pass(4);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();

	airplane->Render();
	kachujin->Render();
	weapon->Render();

	terrain->Pass(7);
	terrain->Render();

	reflection->Render();
	mirrorPlane->Pass(18);
	mirrorPlane->Render();
}

void ReflectionDemo::PostRender()
{
	sky->PostRender();
}

void ReflectionDemo::Mesh()
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
		transform->Position(128, 0, 128);
		transform->Scale(20, 10, 20);
		SetTransform(transform);


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

		mirrorPlane = new MeshRender(shader, new MeshPlane());
		transform = mirrorPlane->AddTransform();
		transform->Position(128, 3, 128);
		transform->Scale(25.6f, 1, 25.6f);

	}

	cube->UpdateTransforms();
	cylinder->UpdateTransforms();
	sphere->UpdateTransforms();
	mirrorPlane->UpdateTransforms();

	meshes.push_back(cube);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
}

void ReflectionDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane"); //TODO. 노멀맵 머티리얼 변경하기
	
	Transform* transform = airplane->AddTransform();
	transform->Scale(0.004f, 0.004f, 0.004f);
	transform->Position(128 + 2.0f, 9.91f, 128 + 2.0f);
	SetTransform(transform);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void ReflectionDemo::Kachujin()
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


void ReflectionDemo::Weapon()
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

void ReflectionDemo::PointLights()
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

void ReflectionDemo::SpotLights()
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


void ReflectionDemo::Pass(UINT val)
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

void ReflectionDemo::SetTransform(Transform * transform)
{
	Vector3 position;
	transform->Position(&position);

	Vector3 scale;
	transform->Scale(&scale);

	float y = terrain->GetHeight(position);
	position.y += y + scale.y * 0.5f;

	transform->Position(position);
}
