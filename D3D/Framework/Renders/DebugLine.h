#pragma once

#define MAX_DEBUG_LINE 150000

class DebugLine
{
public:
	friend class Window;

private:
	DebugLine();
	~DebugLine();

public:
	static void Create();
	static void Delete();
	static DebugLine* Get();

private:
	void Update();
	void Render();

public:
	void RenderLine(Vector3& start, Vector3& end);
	void RenderLine(Vector3& start, Vector3& end, float r, float g, float b);
	void RenderLine(Vector3& start, Vector3& end, Color& color);

	void RenderLine(float x, float y, float z, float x2, float y2, float z2);
	void RenderLine(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b);
	void RenderLine(float x, float y, float z, float x2, float y2, float z2, Color& color);

private:
	static DebugLine* instance;


private:
	struct VertexLine
	{
		Vector3 Position;
		Color Color;
	};

private:
	Shader* shader;
	
	UINT drawCount = 0;

	VertexLine* vertices;
	ID3D11Buffer* vertexBuffer;

	Matrix world;
};
