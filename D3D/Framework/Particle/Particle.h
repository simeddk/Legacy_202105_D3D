#pragma once

class Particle : public Renderer
{
public:
	Particle(wstring file);
	~Particle();

	void Reset();
	void Emit();

	void Update();

private:
	void MapVertices();
	void Activation();
	void Deactivation();

public:
	void Render();

public:
	ParticleData& GetData() { return data; }
	void SetTexture(wstring file);

private:
	void ReadFile(wstring file);

private:
	struct VertexParticle
	{
		Vector3 Position;
		Vector2 Corner;
		Vector3 Velocity;
		Vector4 Random; //x:PlayRate, y:Size, z:Rotate, w:Color
		float Time;
	};

private:
	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float ReadyTime;
		float ReadyRandomTime;

		float CurrentTime;
		float Padding[3];
	} desc;

private:
	ParticleData data;

	Texture* map;
	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexParticle* vertices = nullptr;
	UINT* indices = nullptr;

	float currentTime = 0.0f;
	float lastEmittedTime = 0.0f;

	UINT leadCount = 0;
	UINT gpuCount = 0;
	UINT activeCount = 0;
	UINT deactiveCount = 0;
};