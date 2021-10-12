#include "stdafx.h"
#include "AnimationDemo.h"
#include "Converter.h"

void AnimationDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(5, 3, 0);
	Context::Get()->GetCamera()->Position(5, 1, -4);
	shader = new Shader(L"12_Animation.fxo");

	Kachujin();

	sky = new CubeSky(L"Environment/Mountain1024.dds");
	sky->Pass(2);
	
	planeShader = new Shader(L"09_Mesh.fxo");
	plane = new MeshPlane(planeShader, 6, 6);
	plane->GetTransform()->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");
}

void AnimationDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(kachujin);
	SafeDelete(colliderObject);

	SafeDelete(sky);
	SafeDelete(planeShader);
	SafeDelete(plane);
}

void AnimationDemo::Update()
{
	//AnimationTeset
	{
		static int clip = 0;
		static float speed = 1.0f;
		static float takeTime = 1.0f;

		static bool bBlendMode = false;
		static float blendAlpha = 0.0f;

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
				kachujin->PlayTweenMode(clip, speed, takeTime);
		}
		else //Blend
		{
			ImGui::SliderFloat("Alpha", &blendAlpha, 0.0f, 2.0f);
			kachujin->SetBlendAlpha(blendAlpha);

			if (ImGui::Button("Apply"))
				kachujin->PlayBlendMode(0, 1, 2);
		}
	}
	

	//Light
	{
		static Vector3 LightDirection = Vector3(-1, -1, +1);
		ImGui::SliderFloat3("LightDirection", LightDirection, -1, 1);
		shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
		planeShader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	}

	//Pass
	static UINT pass = 0;
	{
		ImGui::InputInt("Pass", (int*)&pass);
		pass %= 2;
	}

	//Move
	{
		Vector3 P;
		kachujin->GetTransform()->Position(&P);

		if (Keyboard::Get()->Press(VK_UP))
			P += Context::Get()->GetCamera()->Forward() * 10.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			P -= Context::Get()->GetCamera()->Forward() * 10.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_RIGHT))
			P += Context::Get()->GetCamera()->Right() * 10.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			P -= Context::Get()->GetCamera()->Right() * 10.0f * Time::Delta();

		P.y = 0.0f;
		kachujin->GetTransform()->Position(P);
	}

	

	sky->Update();
	plane->Update();
	
	if (kachujin != nullptr)
	{
		kachujin->Pass(pass);
		kachujin->Update();

		kachujin->GetAttachBones(bones);
		colliderObject->World->World(bones[40]);
		colliderObject->Collision->Update();
	}
	
}

void AnimationDemo::Render()
{
	sky->Render();
	plane->Render();

	if (kachujin != nullptr)
	{
		kachujin->Render();
		colliderObject->Collision->Render();
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
	

	kachujin->GetTransform()->Position(0, 0, 0);
	kachujin->GetTransform()->Scale(0.025f, 0.025f, 0.025f);

	colliderObject = new ColliderObject();
	colliderObject->Init->Position(-2.9f, 1.4f, -50.0f);
	colliderObject->Init->Scale(5, 5, 75);
	colliderObject->Init->Rotation(0, 0, 1);
	ZeroMemory(&bones, sizeof(Matrix) * MAX_MODEL_TRANSFORMS);
}
