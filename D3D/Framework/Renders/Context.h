#pragma once

class Context
{
public:
	static Context* Get();
	static void Create();
	static void Delete();

private:
	Context();
	~Context();

public:
	void ResizeScreen();

	void Update();
	void Render();

	D3DXMATRIX View();
	D3DXMATRIX Projection();

	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; }
	class Camera* GetCamera() { return camera; }

	Color& Ambient() { return ambient; }
	Color& Specular() { return specular; }
	Vector3& Direction() { return direction; }
	Vector3& Position() { return position; }

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Viewport* viewport;

	class Camera* camera;

	Color ambient = Color(0, 0, 0, 1);
	Color specular = Color(1, 1, 1, 1);
	Vector3 direction = Vector3(-1, -1, 1);
	Vector3 position = Vector3(0, 0, 0);
};