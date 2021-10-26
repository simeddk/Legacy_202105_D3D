#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	: shader(shader)
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_PerFrame");

	lightBuffer = new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	sLightBuffer = shader->AsConstantBuffer("CB_Light");

	pointLightBuffer = new ConstantBuffer(&pointLightDesc, sizeof(PointLightDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLights");

	spotLightBuffer = new ConstantBuffer(&spotLightDesc, sizeof(SpotLightDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLights");
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
	SafeDelete(lightBuffer);
	SafeDelete(pointLightBuffer);
	SafeDelete(spotLightBuffer);
}

void PerFrame::Update()
{
	desc.Time = Time::Get()->Running();

	desc.View = Context::Get()->View();
	D3DXMatrixInverse(&desc.ViewInverse, nullptr, &desc.View);

	desc.Projection = Context::Get()->Projection();
	desc.VP = desc.View * desc.Projection;

	lightDesc.Ambient = Lighting::Get()->Ambient();
	lightDesc.Specular = Lighting::Get()->Specular();
	lightDesc.Direction = Lighting::Get()->Direction();
	lightDesc.Position = Lighting::Get()->Position();

	pointLightDesc.Count = Lighting::Get()->PointLights(pointLightDesc.Lights);
	spotLightDesc.Count = Lighting::Get()->SpotLights(spotLightDesc.Lights);
}

void PerFrame::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	lightBuffer->Render();
	sLightBuffer->SetConstantBuffer(lightBuffer->Buffer());

	pointLightBuffer->Render();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	spotLightBuffer->Render();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());
}
