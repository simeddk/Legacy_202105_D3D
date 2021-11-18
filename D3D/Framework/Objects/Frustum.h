#pragma once

class Frustum
{
public:
	Frustum(Camera* camera = nullptr, Perspective* perspective = nullptr);
	~Frustum();

	void Update();
	void Planes(Plane* planes, UINT size = 4);

	bool CheckPoint(Vector3& position);

private:
	Plane planes[6];

	Camera* camera;
	Perspective* perspective;
};