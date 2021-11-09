#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file, Shader* shader)
	: shader(shader)
{
	if (this->shader == nullptr)
	{
		this->shader = new Shader(L"00_CubeSky.fxo");
		bCreateShader = true;
	}


	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	Transform* transform = sphere->AddTransform();
	transform->Scale(500, 500, 500);

	sphere->Pass(0);
	
	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(), file.c_str(), nullptr, nullptr, &srv, nullptr
	));

	sSrv = shader->AsSRV("SkyCubeMap");
}

CubeSky::~CubeSky()
{
	if (bCreateShader == true)
		SafeDelete(shader);

	SafeDelete(sphere);
	SafeRelease(srv);
}

void CubeSky::Update()
{
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);

	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();
	sphere->Update();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	
	sphere->Pass(pass);
	sphere->Render();
}
