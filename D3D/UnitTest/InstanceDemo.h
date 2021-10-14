#pragma once
#include "Systems/IExecute.h"

class InstanceDemo : public IExecute
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
	void CreateMesh();

private:
	Shader* shader;
	Material* material;

	vector<Mesh::VertexMesh> vertices;
	vector<UINT> indices;

	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;

	VertexBuffer* instanceBuffer;
	VertexBuffer* colorInstanceBuffer;

	PerFrame* perFrame;
	Transform* transforms[5000];
	Matrix worlds[5000];
	Color colors[5000];
};