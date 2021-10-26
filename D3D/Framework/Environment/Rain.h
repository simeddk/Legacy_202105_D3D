#pragma once

class Rain : public Renderer
{
public:
	Rain(Vector3& extent, UINT count, wstring file);
	~Rain();

	void Update();
	void Render();

private:
	struct Desc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);

		Vector3 Velocity = Vector3(-10, -100, 0);
		float DrawDistance = 0;

		Vector3 Origin = Vector3(0, 0, 0);
		float Padding;

		Vector3 Extent = Vector3(0, 0, 0);
		float Padding2;
	} desc;

private:
	struct VertexRain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Scale;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;
	
	VertexRain* vertices;
	UINT* indices;

	Texture* texture;
	UINT drawCount = 0;
};