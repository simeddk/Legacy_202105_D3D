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

	D3DXMATRIX View() { return view; }
	D3DXMATRIX Projection();

	class Perspective* GetPerspective() { return perspective; }
	class Viewport* GetViewport() { return viewport; }

private:
	static Context* instance;

private:
	class Perspective* perspective;
	class Viewport* viewport;

	D3DXVECTOR3 position;
	D3DXMATRIX view;
};