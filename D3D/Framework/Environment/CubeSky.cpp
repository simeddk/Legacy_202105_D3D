#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file)
{
	shader = new Shader(L"00_CubeSky.fxo");
	sphere = new MeshSphere(shader, 0.5f);
	
	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(), file.c_str(), nullptr, nullptr, &srv, nullptr
	));

	sSrv = shader->AsSRV("SkyCubeMap");
}

CubeSky::~CubeSky()
{
	SafeDelete(shader);
	SafeDelete(sphere);
	SafeRelease(srv);
}

void CubeSky::Update()
{
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);

	sphere->GetTransform()->Position(position);
	sphere->Update();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Render();
}
