#pragma once

class Terrain : public Renderer
{
private:
	struct VertexTerrain;

public:
	Terrain(Shader* shader, wstring imageFile);
	~Terrain();

	void Update();
	void Render();

	void BaseMap(wstring path);
	void Layer1(wstring path);

	UINT Width() { return width; }
	UINT Height() { return height; }

	VertexTerrain* Vertices() { return vertices; }
	void UpdateVertexData();


	float GetHeight(Vector3& position);
	float GetHeight_Raycast(Vector3& position);
	Vector3 GetMousePosition();

private:
	void ReadHeightData();
	void CreateVertexData();
	void CreateIndexData();
	void VisibleNormal();

public:
	void CreateNormalData();


private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv = Vector2(0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
		Vector3 Color = Vector3(0, 0, 0);
	};

private:
	struct Layer
	{
		//높이맵에서 R채널 읽어오기
		float* Data = nullptr;
		ID3D11Texture2D* Texture2D = nullptr;
		ID3D11ShaderResourceView* SRV = nullptr;
		ID3DX11EffectShaderResourceVariable* sSRV;

		//레이어맵
		Texture* Map = nullptr;
		ID3DX11EffectShaderResourceVariable* sMap;

		~Layer()
		{
			SafeDeleteArray(Data);
			SafeRelease(Texture2D);
			SafeRelease(SRV);
			SafeDelete(Map);
		}
	};
	Layer layer1;


private:
	Material* material;
	wstring imageFile;
	float* heights;

	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;
	
	Texture* baseMap = nullptr;

	ID3DX11EffectShaderResourceVariable* sBaseMap;
};