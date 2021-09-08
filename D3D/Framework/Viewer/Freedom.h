#pragma once

class Freedom : public Camera
{
public:
	Freedom();
	~Freedom();

	void Update() override;

	void Speed(float moveSpeed, float rotationSpeed);

private:
	float moveSpeed = 20.0f;
	float rotationSpeed = 2.0f;
};