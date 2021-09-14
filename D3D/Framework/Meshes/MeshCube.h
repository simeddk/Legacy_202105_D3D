#pragma once

class MeshCube : public Mesh
{
public:
	MeshCube(Shader* shader, float offsetU = 1.0f, float offsetV = 1.0f);
	~MeshCube();

private:
	void Create() override;
};