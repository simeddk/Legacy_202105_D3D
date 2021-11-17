#pragma once

class TerrainLOD : public Renderer
{
public:
	TerrainLOD(wstring imageFile);
	~TerrainLOD();

	void Update();
	void Render();

	void BaseMap(wstring file);
	void NormalMap(wstring file);

public:
	float GetWidth();
	float GetHeight();

	float& HeightScale() { return desc.HeightScale; }
	Vector2& Distance() { return desc.Distance; }

private:
	void ReadHeightData();
	void CreatePatchVertex();
	void CreatePatchIndex();

private:
	struct TerrainVertex
	{
		Vector3 Position;
		Vector2 Uv;
	};

private:
	struct Desc
	{
		Vector2 Distance = Vector2(1, 1000);
		Vector2 TessFactor = Vector2(1, 64);

		float CellSpacing = 2.0f;
		float CellSpacingU;
		float CellSpacingV;
		float HeightScale = 1.5f;
	} desc;

private:
	UINT cellsPerPatch = 32;

	wstring imageFile;

	UINT width, height;
	UINT patchWidth, patchHeight;

	float* heights;

	ID3D11Texture2D* heightMap = NULL;
	ID3D11ShaderResourceView* heightMapSRV = NULL;
	ID3DX11EffectShaderResourceVariable* sHeightMapSRV;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	Texture* baseMap = NULL;
	ID3DX11EffectShaderResourceVariable* sBaseMap;

	Texture* normalMap = NULL;
	ID3DX11EffectShaderResourceVariable* sNormalMap;
};