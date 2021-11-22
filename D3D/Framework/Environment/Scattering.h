#pragma once

class Scattering
{
public:
	Scattering(Shader* shader);
	~Scattering();

	void Update();

	void PreRender();
	void PostRender();

	void Pass(UINT val) { pass = val; }

	RenderTarget* RayleighRTV() { return rayleighTarget; }
	RenderTarget* MieRTV() { return mieTarget; }

private:
	void CreateQuad();

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	UINT pass = 0;

	float width, height;

	Shader* shader;
	Render2D* render2D;

	RenderTarget* rayleighTarget, *mieTarget;
	DepthStencil* depthStencil;
	Viewport* viewport;

	VertexBuffer* vertexBuffer;
};