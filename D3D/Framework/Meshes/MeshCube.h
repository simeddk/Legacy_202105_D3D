#pragma once

class MeshCube : public Mesh
{
public:
	MeshCube();
	~MeshCube();

private:
	void Create() override;
};