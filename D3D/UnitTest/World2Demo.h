#pragma once
#include "Systems/IExecute.h"

class World2Demo : public IExecute
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
	Shader* shader;

	Vertex vertices[6];
	ID3D11Buffer* vertexBuffer;

	Color colors[10];
	Matrix worlds[10];
};