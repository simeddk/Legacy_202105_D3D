#pragma once
#include "Systems/IExecute.h"

class FrustumDemo : public IExecute
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
	void CreateMeshData();
	
private:
	Shader* shader;
	Material* red;
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	PerFrame* perFrame;

	vector<Transform *> transforms;

	Shader* gridShader;
	Material* floor;
	MeshRender* plane;

	ModelRender* model;

	Fixity* camera;
	Perspective* perspective;
	Frustum* frustum;

	float fov = 0.25f;
	float zFar = 100.0f;
};