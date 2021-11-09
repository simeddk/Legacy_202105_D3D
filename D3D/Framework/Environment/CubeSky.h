#pragma once

class CubeSky
{
public:
	CubeSky(wstring file, Shader* shader = nullptr);
	~CubeSky();

	void Update();
	void Render();

public:
	Shader* GetShader() { return  shader; }
	void Pass(UINT pass) { this->pass = pass; }

private:
	Shader* shader;
	MeshRender* sphere;

	UINT pass = 0;
	bool bCreateShader = false;

	ID3D11ShaderResourceView* srv;
	ID3DX11EffectShaderResourceVariable* sSrv;
};