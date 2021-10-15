#pragma once

class MeshSphere : public Mesh
{
public:
	MeshSphere(float radius, UINT stackCount = 20, UINT sliceCount = 20);
	~MeshSphere();

private:
	void Create() override;

	float radius;

	UINT stackCount;
	UINT sliceCount;
};