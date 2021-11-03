#include "stdafx.h"
#include "Editor.h"
#include "Utilities/Xml.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	
	shader = new Shader(L"22_AreaLighting.fxo");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	Mesh();

	UpdateParticleList();
	UpdateTextureList();
}

void Editor::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);

	SafeDelete(particle);

	SafeDelete(stone);
	SafeDelete(floor);

	SafeDelete(sphere);
	SafeDelete(grid);
}

void Editor::Update()
{
	OnGUI();

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

	if (particle != nullptr)
	{
		particle->Emit(P);
		particle->Update();
	}
}

void Editor::Render()
{
	sky->Render();

	stone->Render(); //MAT
	sphere->Render(); //MESH

	floor->Render(); //MAT
	grid->Render(); //Mesh

	if (particle != nullptr)
		particle->Render();
}

void Editor::Mesh()
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

void Editor::Pass(UINT meshPass)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(meshPass);
}

void Editor::UpdateParticleList()
{
	particleList.clear();
	Path::GetFiles(&particleList, L"../../_Textures/Particles/", L"*.xml", false);

	for (wstring& file : particleList)
		file = Path::GetFileNameWithoutExtension(file);
}

void Editor::UpdateTextureList()
{
	textureList.clear();

	vector<wstring> files;
	Path::GetFiles(&files, L"../../_Textures/Particles/", L"*.*", false);

	for (wstring file : files)
	{
		wstring ext = Path::GetExtension(file);
		transform(ext.begin(), ext.end(), ext.begin(), toupper);

		file = Path::GetFileName(file);
		if (ext == L"PNG" || ext == L"TGA" || ext == L"JPG")
			textureList.push_back(file);
	}
}

void Editor::OnGUI()
{
	float width = D3D::Width();
	float height = D3D::Height();

	bool bOpen = true;
	bOpen = ImGui::Begin("Particle", &bOpen);
	ImGui::SetWindowPos(ImVec2(width - windowWidth, 0));
	ImGui::SetWindowSize(ImVec2(windowWidth, height));
	{
		OnGUI_List();
		OnGUI_Settings();
	}
	ImGui::End();
}

void Editor::OnGUI_List()
{
	if (ImGui::CollapsingHeader("Particle List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < particleList.size(); i++)
		{
			if (ImGui::Button(String::ToString(particleList[i]).c_str(), ImVec2(200, 0)))
			{
				SafeDelete(particle);

				file = particleList[i];
				particle = new Particle(particleList[i]);

				bLoop = particle->GetData().bLoop;
				maxParticle = particle->GetData().MaxParticles;
			}
		}//for(i)
	}//ImGui::CollapsingHeader
}

void Editor::OnGUI_Settings()
{
	if (particle == NULL) return;

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Particle Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Separator();

		ImGui::SliderInt("MaxParticles", (int *)&maxParticle, 1, 1000);
		ImGui::Checkbox("Loop", &bLoop);

		if (ImGui::Button("Apply"))
		{
			particle->GetData().bLoop = bLoop;
			particle->GetData().MaxParticles = maxParticle;
			particle->Reset();
		}


		ImGui::Separator();

		const char* types[] = { "Opaque", "Additive", "AlphaBlend" };
		ImGui::Combo("BlendType", (int *)&particle->GetData().Type, types, 3);

		ImGui::SliderFloat("ReadyTime", &particle->GetData().ReadyTime, 0.1f, 10.0f);
		ImGui::SliderFloat("ReadyRandomTime", &particle->GetData().ReadyRandomTime, 0.0f, 100.0f);

		ImGui::SliderFloat("StartVelocity", &particle->GetData().StartVelocity, 0.0f, 10.0f);
		ImGui::SliderFloat("EndVelocity", &particle->GetData().EndVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat("MinHorizontalVelocity", &particle->GetData().MinHorizontalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxHorizontalVelocity", &particle->GetData().MaxHorizontalVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat("MinVerticalVelocity", &particle->GetData().MinVerticalVelocity, -100.0f, 100.0f);
		ImGui::SliderFloat("MaxVerticalVelocity", &particle->GetData().MaxVerticalVelocity, -100.0f, 100.0f);

		ImGui::SliderFloat3("Gravity", particle->GetData().Gravity, -100, 100);

		ImGui::ColorEdit4("MinColor", particle->GetData().MinColor);
		ImGui::ColorEdit4("MaxColor", particle->GetData().MaxColor);

		ImGui::SliderFloat("MinRotateSpeed", &particle->GetData().MinRotateSpeed, -10, 10);
		ImGui::SliderFloat("MaxRotateSpeed", &particle->GetData().MaxRotateSpeed, -10, 10);

		ImGui::SliderFloat("MinStartSize", &particle->GetData().MinStartSize, 0, 500);
		ImGui::SliderFloat("MaxStartSize", &particle->GetData().MaxStartSize, 0, 500);

		ImGui::SliderFloat("MinEndSize", &particle->GetData().MinEndSize, 0, 500);
		ImGui::SliderFloat("MaxEndSize", &particle->GetData().MaxEndSize, 0, 500);

		ImGui::Spacing();
		OnGUI_Write();


		ImGui::Spacing();
		ImGui::Separator();
		if (ImGui::CollapsingHeader("TextureList", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (wstring textureFile : textureList)
			{
				if (ImGui::Button(String::ToString(textureFile).c_str(), ImVec2(200, 0)))
				{
					particle->GetData().TextureFile = textureFile;
					particle->SetTexture(L"Particles/" + textureFile);
				}
			}//for(i)
		}
	}
}

void Editor::OnGUI_Write()
{
	ImGui::Separator();

	if (ImGui::Button("WriteParticle"))
	{
		D3DDesc desc = D3D::GetDesc();

		Path::SaveFileDialog
		(
			file, L"Particle file\0*.xml", L"../../_Textures/Particles",
			bind(&Editor::WriteFile, this, placeholders::_1),
			desc.Handle
		);
	}
}

void Editor::WriteFile(wstring file)
{
	Xml::XMLDocument* document = new Xml::XMLDocument();

	Xml::XMLDeclaration* decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Particle");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);


	Xml::XMLElement* node = NULL;

	node = document->NewElement("BlendState");
	node->SetText((int)particle->GetData().Type);
	root->LinkEndChild(node);


	string textureFile = String::ToString(particle->GetData().TextureFile);
	String::Replace(&textureFile, "Particles/", "");

	node = document->NewElement("Loop");
	node->SetText(particle->GetData().bLoop);
	root->LinkEndChild(node);

	node = document->NewElement("TextureFile");
	node->SetText(textureFile.c_str());
	root->LinkEndChild(node);


	node = document->NewElement("MaxParticles");
	node->SetText(particle->GetData().MaxParticles);
	root->LinkEndChild(node);


	node = document->NewElement("ReadyTime");
	node->SetText(particle->GetData().ReadyTime);
	root->LinkEndChild(node);

	node = document->NewElement("ReadyRandomTime");
	node->SetText(particle->GetData().ReadyRandomTime);
	root->LinkEndChild(node);

	node = document->NewElement("StartVelocity");
	node->SetText(particle->GetData().StartVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("EndVelocity");
	node->SetText(particle->GetData().EndVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("MinHorizontalVelocity");
	node->SetText(particle->GetData().MinHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxHorizontalVelocity");
	node->SetText(particle->GetData().MaxHorizontalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MinVerticalVelocity");
	node->SetText(particle->GetData().MinVerticalVelocity);
	root->LinkEndChild(node);

	node = document->NewElement("MaxVerticalVelocity");
	node->SetText(particle->GetData().MaxVerticalVelocity);
	root->LinkEndChild(node);


	node = document->NewElement("Gravity");
	node->SetAttribute("X", particle->GetData().Gravity.x);
	node->SetAttribute("Y", particle->GetData().Gravity.y);
	node->SetAttribute("Z", particle->GetData().Gravity.z);
	root->LinkEndChild(node);


	node = document->NewElement("MinColor");
	node->SetAttribute("R", particle->GetData().MinColor.r);
	node->SetAttribute("G", particle->GetData().MinColor.g);
	node->SetAttribute("B", particle->GetData().MinColor.b);
	node->SetAttribute("A", particle->GetData().MinColor.a);
	root->LinkEndChild(node);

	node = document->NewElement("MaxColor");
	node->SetAttribute("R", particle->GetData().MaxColor.r);
	node->SetAttribute("G", particle->GetData().MaxColor.g);
	node->SetAttribute("B", particle->GetData().MaxColor.b);
	node->SetAttribute("A", particle->GetData().MaxColor.a);
	root->LinkEndChild(node);


	node = document->NewElement("MinRotateSpeed");
	node->SetText(particle->GetData().MinRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MaxRotateSpeed");
	node->SetText(particle->GetData().MaxRotateSpeed);
	root->LinkEndChild(node);

	node = document->NewElement("MinStartSize");
	node->SetText((int)particle->GetData().MinStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxStartSize");
	node->SetText((int)particle->GetData().MaxStartSize);
	root->LinkEndChild(node);

	node = document->NewElement("MinEndSize");
	node->SetText((int)particle->GetData().MinEndSize);
	root->LinkEndChild(node);

	node = document->NewElement("MaxEndSize");
	node->SetText((int)particle->GetData().MaxEndSize);
	root->LinkEndChild(node);

	wstring folder = Path::GetDirectoryName(file);
	wstring fileName = Path::GetFileNameWithoutExtension(file);

	document->SaveFile(String::ToString(folder + fileName + L".xml").c_str());
	SafeDelete(document);

	UpdateParticleList();
}
