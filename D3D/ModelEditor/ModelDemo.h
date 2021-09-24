#pragma once
#include "Systems/IExecute.h"

class ModelDemo : public IExecute
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
	void Tank();
	void Kachujin();
	
private:
	Shader* shader;

	ModelRender* tank = nullptr;
	ModelRender* kachujin = nullptr;

	CubeSky* sky;
	Shader* planeShader;
	MeshPlane* plane;

};