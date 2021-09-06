#pragma once

class Orthographic : public Projection
{
public:
	Orthographic(float width, float height, float zn = 0.1f, float zf = 1000.0f);
	~Orthographic();

	void Set(float width, float height, float zn = 0.1f, float zf = 1000.0f, float fov = 0.0f) override;

private:
	
};