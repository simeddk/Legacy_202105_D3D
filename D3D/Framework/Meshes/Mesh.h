#pragma once

class Mesh : public Renderer
{
public:
	Mesh(Shader* shader);
	virtual ~Mesh();

	void Update();
	void Render();

public:
	void DiffuseMap(wstring file);

protected:
	virtual void Create() = 0;


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

private:
	Texture* diffuseMap = nullptr;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
};