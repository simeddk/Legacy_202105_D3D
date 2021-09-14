#pragma once

class CubeMapMesh
{
public:
	CubeMapMesh(Shader* shader);
	~CubeMapMesh();

	void Texture(wstring file);

	void Update();
	void Render();

public:
public:
	void Pass(UINT val);

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

private:
	Shader* shader;
	UINT pass = 0;

	Mesh* mesh;

	ID3D11ShaderResourceView* srv = nullptr;
	ID3DX11EffectShaderResourceVariable* sSrv;

};