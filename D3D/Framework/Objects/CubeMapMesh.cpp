#include "Framework.h"
#include "CubeMapMesh.h"

CubeMapMesh::CubeMapMesh(Shader * shader)
	: shader(shader)
{
	mesh = new MeshSphere(shader, 0.5f);

	sSrv = shader->AsSRV("CubeMap");
	
}

CubeMapMesh::~CubeMapMesh()
{
	SafeDelete(mesh);
	SafeRelease(srv);
}

void CubeMapMesh::Texture(wstring file)
{
	SafeRelease(srv);

	file = L"../../_Textures/" + file;

	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), file.c_str(), nullptr, nullptr, &srv, nullptr
	));
}

void CubeMapMesh::Update()
{
	mesh->Update();
}

void CubeMapMesh::Render()
{
	sSrv->SetResource(srv);

	mesh->Render();
}

void CubeMapMesh::Pass(UINT val)
{
	mesh->Pass(val);
}
