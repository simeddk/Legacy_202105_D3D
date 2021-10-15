#pragma once

class MeshQuad : public Mesh
{
public:
	MeshQuad();
	~MeshQuad();

private:
	void Create() override;
};