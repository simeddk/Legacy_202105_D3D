#pragma once

class Mesh
{
public:
	Mesh(Shader* shader);
	virtual ~Mesh();

	void Update();
	void Render();

public:
	void Pass(UINT val) { pass = val; }

public:
	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	void Rotation(Vector3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	void RotationDegree(Vector3* vec);

	void Scale(float x, float y, float z);
	void Scale(Vector3& vec);
	void Scale(Vector3* vec);

	Matrix World() { return world; }

	Vector3 Foward();
	Vector3 Up();
	Vector3 Right();

	void DiffuseMap(wstring file);

protected:
	virtual void Create() = 0;

private:
	void CreateBuffer();
	void UpdateWorld();

protected:
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
	VertexMesh* vertices = nullptr;
	UINT* indices = nullptr;

	UINT vertexCount;
	UINT indexCount;

private:
	Shader* shader;
	UINT pass = 0;

	Vector3 position = Vector3(0 , 0, 0);
	Vector3 scale = Vector3(1, 1, 1);
	Vector3 rotation = Vector3(0, 0, 0);

	Matrix world;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	ID3DX11EffectMatrixVariable* sWorld, *sView, *sProjection;

	Texture* diffuseMap = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};