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

	struct LightDesc
	{
		Color Ambient;
		Color Specular;
		Vector3 Direction;
		float Padding;

		Vector3 Position;
		float Padding2;
	} lightDesc;

	struct PointLightDesc
	{
		UINT Count;
		float Padding[3];

		PointLight Lights[MAX_POINT_LIGHTS];
	} pointLightDesc;

	struct SpotLightDesc
	{
		UINT Count;
		float Padding[3];

		SpotLight Lights[MAX_SPOT_LIGHTS];
	} spotLightDesc;

	struct FogDesc
	{
		Color FogColor;
		Vector2 FogDistance;
		float FogDensity;
		UINT FogType;
	} fogDesc;

private:
	Shader* shader;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ConstantBuffer* lightBuffer;
	ID3DX11EffectConstantBuffer* sLightBuffer;

	ConstantBuffer* pointLightBuffer;
	ID3DX11EffectConstantBuffer* sPointLightBuffer;

	ConstantBuffer* spotLightBuffer;
	ID3DX11EffectConstantBuffer* sSpotLightBuffer;

	ConstantBuffer* fogBuffer;
	ID3DX11EffectConstantBuffer* sFogBuffer;
};