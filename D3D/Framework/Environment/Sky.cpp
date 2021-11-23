#include "Framework.h"
#include "Sky.h"
#include "Scattering.h"
#include "Dome.h"
#include "Moon.h"

Sky::Sky(Shader * shader, Vector3 position, Vector3 scale)
	: shader(shader)
{
	scatterDesc.InvWaveLength.x = 1.0f / powf(scatterDesc.WaveLength.x, 4.0f);
	scatterDesc.InvWaveLength.y = 1.0f / powf(scatterDesc.WaveLength.y, 4.0f);
	scatterDesc.InvWaveLength.z = 1.0f / powf(scatterDesc.WaveLength.z, 4.0f);

	scatterDesc.WaveLengthMie.x = powf(scatterDesc.WaveLength.x, -0.84f);
	scatterDesc.WaveLengthMie.y = powf(scatterDesc.WaveLength.y, -0.84f);
	scatterDesc.WaveLengthMie.z = powf(scatterDesc.WaveLength.z, -0.84f);

	scattering = new Scattering(shader);
	scatterBuffer = new ConstantBuffer(&scatterDesc, sizeof(ScatterDesc));
	sScatterBuffer = shader->AsConstantBuffer("CB_Scattering");

	sRayleighMap = shader->AsSRV("RayleighMap");
	sMieMap = shader->AsSRV("MieMap");

	dome = new Dome(shader, position, scale);
	moon = new Moon(shader);
}

Sky::~Sky()
{
	SafeDelete(dome);
	SafeDelete(scattering);
	SafeDelete(scatterBuffer);
	SafeDelete(moon);
}

void Sky::Pass(UINT scatteringPass, UINT domePass, UINT moonPass)
{
	scattering->Pass(scatteringPass);
	dome->Pass(domePass);
	moon->Pass(moonPass);
}

void Sky::Pass(UINT pass)
{
	scattering->Pass(pass++);
	dome->Pass(pass++);
	moon->Pass(pass++);
}

void Sky::Update()
{
	if (bRealTime == true)
	{
		ImGui::SliderFloat("Phi", &phi, -Math::PI, Math::PI);

		theta += Time::Delta() * timeFactor;

		if (theta > Math::PI)
			theta -= Math::PI * 2.0f;

		float x = sinf(theta) * cosf(phi);
		float y = cosf(theta);
		float z = sinf(theta) * sinf(phi);

		Lighting::Get()->Direction() = Vector3(x, y, z);
	}
	else
	{
		ImGui::SliderFloat("Theta", &theta, -Math::PI, Math::PI);
		ImGui::SliderFloat("Phi", &phi, -Math::PI, Math::PI);

		float x = sinf(theta) * cosf(phi);
		float y = cosf(theta);
		float z = sinf(theta) * sinf(phi);

		Lighting::Get()->Direction() = Vector3(x, y, z);
	}

	dome->Update();
	moon->Update();
}

void Sky::PreRender()
{
	scatterBuffer->Render();
	sScatterBuffer->SetConstantBuffer(scatterBuffer->Buffer());

	scattering->PreRender();
}

void Sky::Render()
{
	//Dome
	{
		sRayleighMap->SetResource(scattering->RayleighRTV()->SRV());
		sMieMap->SetResource(scattering->MieRTV()->SRV());

		dome->Render();
	}

	//Moon
	{
		moon->Render(theta);
	}
}

void Sky::PostRender()
{
	scattering->PostRender();
}

void Sky::RealTime(bool val, float timeFactor)
{
	bRealTime = val;
	this->timeFactor = timeFactor;
}
