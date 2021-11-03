#pragma once
#include "Systems/IExecute.h"

class Viewer : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {};
	virtual void Render() override;
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	void Mesh();
	void Pass(UINT meshPass);
	
private:
	Particle* particle;

	Shader* shader;
	CubeSky* sky;

	Material* floor;
	Material* stone;

	MeshRender* grid;
	MeshRender* sphere;

	vector<MeshRender*> meshes;
};