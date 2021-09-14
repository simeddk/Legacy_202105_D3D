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

void CubeMapMesh::Position(float x, float y, float z)
{
	mesh->Position(x, y, z);
}

void CubeMapMesh::Position(Vector3 & vec)
{
	mesh->Position(vec);
}

void CubeMapMesh::Position(Vector3 * vec)
{
	mesh->Position(vec);
}

void CubeMapMesh::Rotation(float x, float y, float z)
{
	mesh->Rotation(x, y, z);
}

void CubeMapMesh::Rotation(Vector3 & vec)
{
	mesh->Rotation(vec);
}

void CubeMapMesh::Rotation(Vector3 * vec)
{
	mesh->Rotation(vec);
}

void CubeMapMesh::RotationDegree(float x, float y, float z)
{
	mesh->RotationDegree(x, y, z);
}

void CubeMapMesh::RotationDegree(Vector3 & vec)
{
	mesh->RotationDegree(vec);
}

void CubeMapMesh::RotationDegree(Vector3 * vec)
{
	mesh->RotationDegree(vec);
}

void CubeMapMesh::Scale(float x, float y, float z)
{
	mesh->Scale(x, y, z);
}

void CubeMapMesh::Scale(Vector3 & vec)
{
	mesh->Scale(vec);
}

void CubeMapMesh::Scale(Vector3 * vec)
{
	mesh->Scale(vec);
}