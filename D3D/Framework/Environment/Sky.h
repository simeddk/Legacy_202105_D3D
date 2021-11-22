#pragma once

class Sky
{
public:
	Sky(Shader* shader, Vector3 position = Vector3(128, 16, 128), Vector3 scale = Vector3(140, 140, 140));
	~Sky();

	void Pass(UINT scatteringPass, UINT domePass, UINT moonPass);
	void Pass(UINT  pass);

	void Update();
	void PreRender();
	void Render();
	void PostRender();

public:
	void RealTime(bool val, float timeFactor = 1.0f);
	class Dome* GetDome() { return dome; }

private:
	struct ScatterDesc
	{
		Vector3 WaveLength = Vector3(0.65f, 0.57f, 0.475f);
		float Padding;

		Vector3 InvWaveLength;
		int SampleCount = 8;

		Vector3 WaveLengthMie;
		float Padding2;
	} scatterDesc;

private:
	Shader* shader;

	bool bRealTime = false;

	float timeFactor = 1.0f;
	float theta = Math::PI;
	float phi = 2.0f;

	class Scattering* scattering;
	ConstantBuffer* scatterBuffer;
	ID3DX11EffectConstantBuffer* sScatterBuffer;

	ID3DX11EffectShaderResourceVariable* sRayleighMap;
	ID3DX11EffectShaderResourceVariable* sMieMap;

	class Dome* dome;
};