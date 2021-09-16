#pragma once

class Terrain : public Renderer
{
public:
	Terrain(Shader* shader, wstring heightMapFile);
	~Terrain();

	void Update();
	void Render();

	void BaseMap(wstring path);

	float GetHeight(Vector3& position);
	float GetHeight_Raycast(Vector3& position);

private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

	void VisibleNormal();

private:
	struct VertexTerrain
	{
		Vector3 Position;
		Vector2 Uv = Vector2(0, 0);
		Vector3 Normal = Vector3(0, 0, 0);
	};

private:
	UINT width, height;

	VertexTerrain* vertices;
	UINT* indices;

	Texture* heightMap;
	Texture* baseMap = nullptr;
};