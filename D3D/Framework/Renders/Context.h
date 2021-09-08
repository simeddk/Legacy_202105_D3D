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

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Viewport* viewport;

	class Camera* camera;
};