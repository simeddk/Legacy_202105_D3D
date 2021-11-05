#pragma once

class PostProcess : public Renderer
{
public:
	PostProcess(wstring shaderFile);
	~PostProcess();

	void Update();
	void Render();

	void SRV(ID3D11ShaderResourceView* srv);

private:
	struct Vertex
	{
		Vector3 Position;
	};

private:
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};