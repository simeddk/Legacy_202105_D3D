#pragma once

class Terrain
{
public:
	Terrain(Shader* shader, wstring heightMapFile);
	~Terrain();

	void Update();
	void Render();

	UINT& Pass() { return pass; }

	void BaseMap(wstring path);

	float GetHeight(Vector3& position);
	float GetHeight_Raycast(Vector3& position);

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();
	void CreateBuffer();

private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv = Vector2(0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
	};

private:
	Shader* shader;
	UINT pass = 0;

	UINT width, height;

	UINT vertexCount;
	VertexTerrain* vertices;
	ID3D11Buffer* vertexBuffer;

	UINT indexCount;
	UINT* indices;
	ID3D11Buffer* indexBuffer;

	Matrix world;

	Texture* heightMap;
	Texture* baseMap = nullptr;
};