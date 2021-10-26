#pragma once
#include "Systems/IExecute.h"

class WeatherDemo : public IExecute
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
	void Airplane();
	void Kachujin();
	void Weapon();

	void PointLights();
	void SpotLights();
	
	void Billboards();
	void Weather();

	void Pass(UINT val);
	
private:
	Shader* shader;
	CubeSky* sky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;
	
	MeshRender* cube;
	MeshRender* plane;
	MeshRender* cylinder;
	MeshRender* sphere;

	ModelRender* airplane;

	ModelAnimator* kachujin;
	ModelRender* weapon;
	Transform* weaponTransform;
	
	Billboard* billboard;
	Rain* rain;

	vector<MeshRender*> meshes;
	vector<ModelRender*> models;
	vector<ModelAnimator*> animators;

};