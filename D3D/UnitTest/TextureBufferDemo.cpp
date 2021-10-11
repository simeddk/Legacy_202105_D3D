#include "stdafx.h"
#include "TextureBufferDemo.h"

void TextureBufferDemo::Initialize()
{
	shader = new Shader(L"14_TextureBuffer.fxo");
	texture = new Texture(L"Environment/Compute.png");

	textureBuffer = new TextureBuffer(texture->GetTexture());
	shader->AsSRV("Input")->SetResource(textureBuffer->SRV());
	shader->AsUAV("Output")->SetUnorderedAccessView(textureBuffer->UAV());

	render2D = new Render2D();
	render2D->GetTransform()->Scale(D3D::Width(), D3D::Height(), 1);
	render2D->GetTransform()->Position(D3D::Width() * 0.5f, D3D::Height() * 0.5f, 0);
	render2D->SRV(textureBuffer->OutputSRV());
}

void TextureBufferDemo::Destroy()
{
	SafeDelete(texture);
	SafeDelete(textureBuffer);
	SafeDelete(render2D);
}

void TextureBufferDemo::Update()
{
	UINT width = textureBuffer->Width();
	UINT height = textureBuffer->Height();

	float x = (width / 32.0f) < 1.0f ? 1.0f : ((float)width / 32.0f);
	float y = (height / 32.0f) < 1.0f ? 1.0f : ((float)height / 32.0f);
	UINT groupX = (UINT)ceilf(x);
	UINT groupY = (UINT)ceilf(y);

	static int pass = 0;
	ImGui::SliderInt("Pass", &pass, 0, 2);
	shader->Dispatch(0, pass, groupX, groupY, 1);

	render2D->Update();
}

void TextureBufferDemo::Render()
{
	render2D->Render();
}
