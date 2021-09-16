#pragma once

class CubeMapMesh
{
public:
	CubeMapMesh(Shader* shader);
	~CubeMapMesh();

	void Texture(wstring file);

	void Update();
	void Render();

public:
	void Pass(UINT val);

	Transform* GetTransform() { return mesh->GetTransform(); }

private:
	Shader* shader;
	UINT pass = 0;

	Mesh* mesh;

	ID3D11ShaderResourceView* srv = nullptr;
	ID3DX11EffectShaderResourceVariable* sSrv;

};