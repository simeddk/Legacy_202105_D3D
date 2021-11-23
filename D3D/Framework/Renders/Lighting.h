#pragma once

#define MAX_POINT_LIGHTS 256
struct PointLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	float Intensity;
	float Padding[3];
};

#define MAX_SPOT_LIGHTS 256
struct SpotLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	Vector3 Position;
	float Range;

	Vector3 Direction;
	float Angle;

	float Intensity;
	float Padding[3];
};

class Lighting
{
public:
	static Lighting* Get();
	static void Create();
	static void Delete();

public:
	void Update();

public:
	Color& Ambient() { return ambient; }
	Color& Specular() { return specular; }
	Vector3& Direction() { return direction; }
	Vector3& Position() { return position; }

public:
	UINT PointLightCount() { return pointLightCount; }
	UINT PointLights(OUT PointLight* lights);
	void AddPointLight(PointLight& light);
	PointLight& GetPointLight(UINT index);
	Transform* GetPointLightTransform(UINT index);

public:
	UINT SpotLightCount() { return spotLightCount; }
	UINT SpotLights(OUT SpotLight* lights);
	void AddSpotLight(SpotLight& light);
	SpotLight& GetSpotLight(UINT index);
	Transform* GetSpotLightTransform(UINT index);

public:
	Color& FogColor() { return fogColor; }
	Vector2& FogDistance() { return fogDistance; }
	float& FogDensity() { return fogDensity; }
	UINT& FogType() { return fogType; }

private:
	Lighting();
	~Lighting();

private:
	static Lighting* instance;

	
private: //GlobalLight
	Color ambient = Color(0, 0, 0, 1);
	Color specular = Color(1, 1, 1, 1);
	Vector3 direction = Vector3(-1, -1, 1);
	Vector3 position = Vector3(0, 0, 0);

private: //PointLight
	UINT pointLightCount = 0;
	PointLight pointLights[MAX_POINT_LIGHTS];
	vector<Transform*> pointLightTransforms;

private: //SpotLight
	UINT spotLightCount = 0;
	SpotLight spotLights[MAX_SPOT_LIGHTS];
	vector<Transform*> spotLightTransforms;

private: //Fog
	Color fogColor = Color(0.75f, 0.75f, 0.75f, 1);
	Vector2 fogDistance = Vector2(1, 105);
	float fogDensity = 0.25f;
	UINT fogType = 2;
};