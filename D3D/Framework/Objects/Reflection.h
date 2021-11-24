#pragma once

class Reflection
{
public:
	Reflection(Shader* shader, Transform* transform, float width = 0, float height = 0);
	~Reflection();

	void Update();

	void PreRender();
	void Render();

private:
	Shader* shader;
	Transform* transform;
	Fixity* camera;

	float width, height;
	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ID3DX11EffectShaderResourceVariable* sReflectionSRV;
	ID3DX11EffectMatrixVariable* sReflectionView;

};