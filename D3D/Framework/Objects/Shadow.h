#pragma once

class Shadow
{
public:
	Shadow(Shader* shader, Vector3& at, float radius, float width = 1024, float height = 1024);
	~Shadow();

	void PreRender();

	ID3D11ShaderResourceView* SRV() { return renderTarget->SRV(); }

	void Quality(UINT val) { desc.Quality = val; }
	void Bias(float val) { desc.Bias = val; }

private:
	struct Desc
	{
		Matrix View;
		Matrix Projection;

		Vector2 MapSize;
		float Bias = 0.0f;

		UINT Quality = 2;

	} desc;

private:
	Shader* shader;
	
	float width, height;

	float radius;
	Vector3 at;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3D11SamplerState* sampler;
	ID3DX11EffectSamplerVariable* sSampler;

	ID3DX11EffectShaderResourceVariable* sShadowMap;
};
