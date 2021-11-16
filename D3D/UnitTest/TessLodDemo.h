#pragma once
#include "Systems/IExecute.h"

class TessLodDemo : public IExecute
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
	struct Vertex
	{
		Vector3 Position;
	};

private:
	Shader* shader;

	PerFrame* perFrame;

	VertexBuffer* vertexBuffer;
};