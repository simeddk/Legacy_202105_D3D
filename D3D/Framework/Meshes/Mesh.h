#pragma once

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

	void Update();
	void Render(UINT instanceCount);

	void SetShader(Shader* shader);
	void Pass(UINT val) { pass = val; }


protected:
	virtual void Create() = 0;


public:
	struct VertexMesh
	{
		Vector3 Position;
		Vector2 Uv;
		Vector3 Normal;

		VertexMesh()
		{
			Position = Vector3(0, 0, 0);
			Uv = Vector2(0, 0);
			Normal = Vector3(0, 0, 0);
		}

		VertexMesh(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		{
			Position = Vector3(x, y, z);
			Uv = Vector2(u, v);
			Normal = Vector3(nx, ny, nz);
		}

		VertexMesh(Vector3 position, Vector2 uv, Vector3 normal)
		{
			Position = position;
			Uv = uv;
			Normal = normal;
		}
	};

protected:
	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = nullptr;

	VertexBuffer* vertexBuffer = nullptr;
	IndexBuffer* indexBuffer = nullptr;

	VertexMesh* vertices = nullptr;
	UINT* indices = nullptr;

	UINT vertexCount, indexCount;


};