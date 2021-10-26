#pragma once

#define MAX_BILLBOARD_CONUT 10000

class Billboard : public Renderer
{
public:
	Billboard(wstring textureFile);
	~Billboard();

	void Render();

	void Add(Vector3& position, Vector2& scale);

private:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Scale;
	};

private:
	VertexBillboard* vertices;
	UINT* indices;

	UINT prevCount = 0;
	UINT drawCount = 0;

	Texture* diffuseMap;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};