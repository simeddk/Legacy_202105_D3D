#pragma once

class Water : public Renderer
{
public:
	struct InitializeDesc;

public:
	Water(InitializeDesc& initDesc);
	~Water();

	void Update();

	void PreRender_Refraction();
	void PreRender_Reflection();
	void Render();

public:
	void NormalMap(wstring file);
	void WaterMap(wstring file);

	function<void(wstring)> OnButtonPressed_WaterMap;

public:
	struct InitializeDesc
	{
		Shader* Shader;

		float Radius;
		wstring HeightFile;
		float Width = 0.0f;
		float Height = 0.0f;
	} initDesc;


private:
	struct Desc
	{
		Color RefractionColor = Color(0.2f, 0.3f, 1.0f, 1.0f);

		Vector2 NormalMapTile = Vector2(4, 8);
		float WaveTranlation = 0.0f;
		float WaveScale = 0.05f;

		float Shiness = 30.0f;
		float Alpha = 0.5f;
		float WaveSpeed = 0.06f;
		float WaterPositionY;

		float TerrainWidth;
		float TerrainHeight;
		float WaterWidth;
		float WaterHeight;
	} desc;

private:
	struct VertexTexture
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	class Refraction* refraction;
	class Reflection* reflection;

	Texture* heightMap;
	ID3DX11EffectShaderResourceVariable* sHeightMap;

	Texture* waterMap;
	ID3DX11EffectShaderResourceVariable* sWaterMap;
};
