#pragma once

class Moon : public Renderer
{
public:
	Moon(Shader* shader);
	~Moon();

	void Update();
	void Render(float theta);

private:
	float GetAlpha(float theta);

	Matrix GetTransform(float theta);
	Matrix GetGlowTransform(float theta);

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	float scale = 2.5f;
	float distance = 25;

	float glowScale = 3.5f;
	float glowDistance = 20;

	ID3DX11EffectScalarVariable* sAlpha;

	Texture* moon;
	Texture* moonGlow;
	ID3DX11EffectShaderResourceVariable* sMoon;
};