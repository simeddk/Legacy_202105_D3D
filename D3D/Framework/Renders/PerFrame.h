#pragma once

class PerFrame
{
public:
	PerFrame(Shader* shader);
	~PerFrame();

	void Update();
	void Render();

private:
	struct Desc
	{
		Matrix View;
		Matrix ViewInverse;
		Matrix Projection;
		Matrix VP;

		Plane Culling[4];
		Plane Clipping;

		float Time;
		float Padding[3];
	} desc;

private:
	Shader* shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

};