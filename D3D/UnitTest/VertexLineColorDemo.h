#pragma once
#include "Systems/IExecute.h"

class VertexLineColorDemo : public IExecute
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
	struct VertexColor
	{
		Vector3 Position;
		Color Color;
	};

private:
	Shader* shader;

	VertexColor vertices[2];
	ID3D11Buffer* vertexBuffer;

};