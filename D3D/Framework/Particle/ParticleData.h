#pragma once
#include "Framework.h"

class ParticleData
{
public:
	enum class BlendType
	{
		Opaque = 0, Additive, AlphaBlend
	} Type = BlendType::Opaque;

	bool bLoop = false;

	wstring TextureFile = L"";

	UINT MaxParticles = 100;

	float ReadyTime = 1.0f;
	float ReadyRandomTime = 0.0f;

	float StartVelocity = 1.0f;
	float EndVelocity = 1.0f;

	float MinHorizontalVelocity = 0.0f;
	float MaxHorizontalVelocity = 0.0f;

	float MinVerticalVelocity = 0.0f;
	float MaxVerticalVelocity = 0.0f;

	Vector3 Gravity = Vector3(0, 0, 0);

	Color MinColor = Color(1, 1, 1, 1);
	Color MaxColor = Color(1, 1, 1, 1);

	float MinRotateSpeed = 0.0f;
	float MaxRotateSpeed = 0.0f;

	float MinStartSize = 0.0f;
	float MaxStartSize = 0.0f;

	float MinEndSize = 0.0f;
	float MaxEndSize = 0.0f;
};