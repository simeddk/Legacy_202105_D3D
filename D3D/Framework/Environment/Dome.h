#pragma once

class Dome : public Renderer
{
public:
	Dome(Shader* shader, Vector3 position, Vector3 scale, UINT drawCount = 32);
	~Dome();

	void Update() override;
	void Render() override;

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	UINT drawCount;

	Texture* starMap;
	ID3DX11EffectShaderResourceVariable* sStarMap;
};