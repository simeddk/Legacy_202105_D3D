#include "Framework.h"
#include "Water.h"

Water::Water(InitializeDesc & initDesc)
	: Renderer(initDesc.Shader)
	, initDesc(initDesc)
{
	vertexCount = 4;
	indexCount = 6;

	VertexTexture vertices[4];
	vertices[0].Position = Vector3(-initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[1].Position = Vector3(-initDesc.Radius, 0.0f, +initDesc.Radius);
	vertices[2].Position = Vector3(+initDesc.Radius, 0.0f, -initDesc.Radius);
	vertices[3].Position = Vector3(+initDesc.Radius, 0.0f, +initDesc.Radius);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 0);

	UINT indices[6] = { 0, 1, 2, 2, 1, 3 };

	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexTexture));
	indexBuffer = new IndexBuffer(indices, indexCount);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Water");

	this->initDesc.Width = initDesc.Width > 0 ? initDesc.Width : D3D::Width();
	this->initDesc.Height = initDesc.Height > 0 ? initDesc.Height : D3D::Height();

	heightMap = new Texture(initDesc.HeightFile);
	sHeightMap = shader->AsSRV("HeightMap");

	waterMap = new Texture(L"Environment/Water3.png");
	sWaterMap = shader->AsSRV("WaterMap");

	OnButtonPressed_WaterMap = bind(&Water::WaterMap, this, placeholders::_1);

	reflection = new Reflection(shader, transform, this->initDesc.Width, this->initDesc.Height);
	refraction = new Refraction(shader, transform, L"Environment/Wave2.dds", this->initDesc.Width, this->initDesc.Height);
}

Water::~Water()
{
	SafeDelete(buffer);
	SafeDelete(reflection);
	SafeDelete(refraction);
	SafeDelete(heightMap);
	SafeDelete(waterMap);
}

void Water::Update()
{
	Super::Update();

	desc.WaveTranlation += sin(desc.WaveScale * Time::Delta());

	refraction->Update();
	reflection->Update();
}

void Water::PreRender_Reflection()
{
	reflection->PreRender();
}

void Water::PreRender_Refraction()
{
	refraction->PreRender();
}

void Water::Render()
{
	Super::Render();

	refraction->Render();
	reflection->Render();

	desc.TerrainWidth = (float)heightMap->GetWidth();
	desc.TerrainHeight = (float)heightMap->GetHeight();

	Vector3 scale, position;
	GetTransform()->Scale(&scale);

	desc.WaterWidth = scale.x * initDesc.Radius * 2.0f;
	desc.WaterHeight = scale.y * initDesc.Radius * 2.0f;

	GetTransform()->Position(&position);

	desc.WaterPositionY = position.y;

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sHeightMap->SetResource(heightMap->SRV());
	sWaterMap->SetResource(waterMap->SRV());

	shader->DrawIndexed(0, Pass(), indexCount);
}

void Water::NormalMap(wstring file)
{
	refraction->NormalMap(file);
}

void Water::WaterMap(wstring file)
{
	SafeDelete(waterMap);
	waterMap = new Texture(file);
}