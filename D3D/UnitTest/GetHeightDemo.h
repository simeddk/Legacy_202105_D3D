#pragma once
#include "Systems/IExecute.h"

class GetHeightDemo : public IExecute
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
	Shader* terrainShader;

	Terrain* terrain;

	Shader* triangleShader;
	ID3D11Buffer* vertexBuffer;
	Vector3 position = Vector3(0, 0, 0);


};