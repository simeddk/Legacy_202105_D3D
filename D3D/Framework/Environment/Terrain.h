#pragma once

class Terrain
{
public:
	Terrain(Shader* shader, wstring heightMapFile);
	~Terrain();

	void Update();
	void Render();

	UINT& Pass() { return pass; }

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateBuffer();

private:
	struct VertexTerrain
	{
		Vector3 Position;
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
};