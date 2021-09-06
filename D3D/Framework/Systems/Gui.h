#pragma once

struct GuiText
{
	D3DXVECTOR2 Position;
	D3DXCOLOR Color;
	string Content;

	GuiText()
	{

	}
};

class Gui
{
public:
	static void Create();
	static void Delete();

	static Gui* Get();

	LRESULT MsgProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
	void Resize();

	void Update();
	void Render();

	void RenderText(GuiText& text);
	void RenderText(float x, float y, string content);
	void RenderText(float x, float y, float r, float g, float b, string content);
	void RenderText(D3DXVECTOR2 position, D3DXCOLOR color, string content);

private:
	Gui();
	~Gui();

private:
	void ApplyStyle();

private:
	static Gui* instance;

	vector<GuiText> texts;
};