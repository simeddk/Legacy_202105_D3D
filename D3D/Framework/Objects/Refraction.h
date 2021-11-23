#pragma once

class Refraction
{
public:
	Refraction(Shader* shader, Transform* transform, wstring normalFile, float width = 0, float height = 0);
	~Refraction();

	void Update();

	void PreRender();
	void Render();

	void NormalMap(wstring normalFile);

public:
	function<void(wstring)> OnButtonPressed_NormalMap;

private:
	Shader* shader;
	Transform* transform;

	float width, height;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sRefractionSRV;

	Texture* normalMap;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};
