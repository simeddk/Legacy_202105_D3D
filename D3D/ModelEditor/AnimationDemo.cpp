#include "stdafx.h"
#include "AnimationDemo.h"
#include "Converter.h"

void AnimationDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(19, 0, 0);
	Context::Get()->GetCamera()->Position(0, 5, -8);

	shader = new Shader(L"19_Surface.fxo");

	Kachujin();

}

void AnimationDemo::Destroy()
{
	SafeDelete(shader);

	for (UINT i = 0 ; i < kachujin->TransformCount(); i++)
		SafeDelete(colliderObject[i]);
	SafeDeleteArray(colliderObject);

	SafeDelete(kachujin);
	SafeDelete(weapon);
	SafeDelete(weaponInitTransform);
	
}

void AnimationDemo::Update()
{
	//AnimationTest
	{
		static UINT instance = 20;
		static int clip = 0;
		static float speed = 1.0f;
		static float takeTime = 0.1f;

		static bool bBlendMode = false;
		static float blendAlpha = 0.0f;

		ImGui::InputInt("KachujinID", (int*)&instance);
		ImGui::Checkbox("BlendMode", &bBlendMode);
		if (bBlendMode == false) //Tween
		{
			ImGui::InputInt("Clip", &clip);
			clip = Math::Clamp<int>(clip, 0, 4);

			const char* clipName[] = { "Idle", "Walk", "Run", "Slash", "Uprock" };
			ImGui::Text("%s", clipName[clip]);
			ImGui::SliderFloat("Speed", &speed, 0.1f, 5.0f);
			ImGui::SliderFloat("TakeTime", &takeTime, 0.1f, 5.0f);

			if (ImGui::Button("Apply"))
				kachujin->PlayTweenMode(instance, clip, speed, takeTime);
		}
		else //Blend
		{
			ImGui::SliderFloat("Alpha", &blendAlpha, 0.0f, 2.0f);
			kachujin->SetBlendAlpha(instance, blendAlpha);

			if (ImGui::Button("Apply"))
				kachujin->PlayBlendMode(instance, 0, 1, 2);
		}
	}
	

	//Light
	{
		static Vector3 LightDirection = Vector3(-1, -1, +1);
		ImGui::SliderFloat3("LightDirection", LightDirection, -1, 1);
		shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	}
	
	
	if (kachujin != nullptr)
	{
		UINT count = kachujin->TransformCount();
		for (UINT i = 0; i < count; i++)
		{
			kachujin->GetAttachBones(i, bones);

			Transform* weaponTransform = weapon->GetTransform(i);
			weaponTransform->World(weaponInitTransform->World() * bones[40]);

			colliderObject[i]->World->World(bones[40]);
			colliderObject[i]->Collision->Update();
		}
		weapon->UpdateTransforms();

		kachujin->Update();
		weapon->Update();

	}
	
}

void AnimationDemo::Render()
{
	
	if (kachujin != nullptr)
	{
		UINT count = kachujin->TransformCount();

		for (UINT i = 0; i < count; i++)
			colliderObject[i]->Collision->Render();

		kachujin->Pass(2);
		kachujin->Render();

		weapon->Pass(1);
		weapon->Render();
	}

}


void AnimationDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");
	
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = kachujin->AddTransform();
		transform->Position(x, 0, -5);
		transform->Scale(0.01f, 0.01f, 0.01f);
	}
	kachujin->UpdateTransforms();

 	UINT count = kachujin->TransformCount();

	//Collider
	{
		colliderObject = new ColliderObject*[count];
		for (UINT i = 0; i < count; i++)
		{
			colliderObject[i] = new ColliderObject();
			colliderObject[i]->Init->Position(-2.9f, 1.4f, -50.0f);
			colliderObject[i]->Init->Scale(5, 5, 75);
		}
	}

	//Weapon
	{
		weapon = new ModelRender(shader);
		weapon->ReadMesh(L"Weapon/Sword");
		weapon->ReadMaterial(L"Weapon/Sword");
		for (UINT i = 0; i < count; i++)
			weapon->AddTransform();

		weaponInitTransform = new Transform();
		weaponInitTransform->Position(-2.9f, 1.45f, -6.45f);
		weaponInitTransform->Scale(0.5f, 0.5f, 0.75f);
		weaponInitTransform->Rotation(0, 0, 1);
	}
	
}
