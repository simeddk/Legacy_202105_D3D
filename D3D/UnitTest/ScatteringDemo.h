#pragma once
#include "Systems/IExecute.h"

class ScatteringDemo : public IExecute
{
public:
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;;
	virtual void ResizeScreen() override {};

private:
	void Mesh();
	void Airplane();
	void Kachujin();
	void Weapon();

	void PointLights();
	void SpotLights();

	void Pass(UINT val);
	void SetTransform(Transform* transform);
	
private:
	Shader* shader;
	Shadow* shadow;

	Sky* sky;
	Terrain* terrain;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;
	
	MeshRender* cube;
	MeshRender* cylinder;
	MeshRender* sphere;

	ModelRender* airplane;

	ModelAnimator* kachujin;
	ModelRender* weapon;
	Transform* weaponTransform;
	
	vector<MeshRender*> meshes;
	vector<ModelRender*> models;
	vector<ModelAnimator*> animators;

};