#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	: shader(shader)
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_PerFrame");
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
}

void PerFrame::Update()
{
	desc.Time = Time::Get()->Running();

	desc.View = Context::Get()->View();
	D3DXMatrixInverse(&desc.ViewInverse, nullptr, &desc.View);

	desc.Projection = Context::Get()->Projection();
	desc.VP = desc.View * desc.Projection;
}

void PerFrame::Render()
{
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}
