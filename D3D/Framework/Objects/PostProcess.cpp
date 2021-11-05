#include "Framework.h"
#include "PostProcess.h"

PostProcess::PostProcess(wstring shaderFile)
	: Renderer(shaderFile)
{
	Vertex vertices[6];
	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(Vertex));
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

PostProcess::~PostProcess()
{
}

void PostProcess::Update()
{
	Super::Update();
}

void PostProcess::Render()
{
	Super::Render();

	shader->Draw(0, Pass(), 6);
}

void PostProcess::SRV(ID3D11ShaderResourceView * srv)
{
	sDiffuseMap->SetResource(srv);
}
