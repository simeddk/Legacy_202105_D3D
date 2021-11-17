#include "Framework.h"
#include "TerrainLOD.h"

TerrainLOD::TerrainLOD(wstring imageFile)
	: Renderer(L"39_TerrainLOD.fxo"), imageFile(imageFile)
{
	Topology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	ReadHeightData();

	patchWidth = ((width - 1) / cellsPerPatch) + 1;
	patchHeight = ((height - 1) / cellsPerPatch) + 1;

	vertexCount = patchWidth * patchHeight;
	indexCount = (patchWidth - 1) * (patchHeight - 1);

	CreatePatchVertex();
	CreatePatchIndex();

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_TerrainLOD");

	sHeightMapSRV = shader->AsSRV("HeightMap");
	sBaseMap = shader->AsSRV("BaseMap");
	sNormalMap = shader->AsSRV("NormalMap");

	desc.CellSpacingU = 1.0f / width;
	desc.CellSpacingV = 1.0f / height;
}

TerrainLOD::~TerrainLOD()
{
	SafeDeleteArray(heights);

	SafeRelease(heightMap);
	SafeRelease(heightMapSRV);

	SafeDelete(buffer);

	SafeDelete(baseMap);
}

void TerrainLOD::Update()
{
	Super::Update();
}

void TerrainLOD::Render()
{
	Super::Render();

	sHeightMapSRV->SetResource(heightMapSRV);


	if (baseMap != NULL)
		sBaseMap->SetResource(baseMap->SRV());

	if (normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	shader->DrawIndexed(0, Pass(), indexCount * 4);
}

void TerrainLOD::BaseMap(wstring file)
{
	SafeDelete(baseMap);
	baseMap = new Texture(file);
}

void TerrainLOD::NormalMap(wstring file)
{
	SafeDelete(normalMap);
	normalMap = new Texture(file);
}

float TerrainLOD::GetWidth()
{
	return width * desc.CellSpacing;
}

float TerrainLOD::GetHeight()
{
	return height * desc.CellSpacing;
}

void TerrainLOD::ReadHeightData()
{
	wstring ext = Path::GetExtension(imageFile);
	std::transform(ext.begin(), ext.end(), ext.begin(), toupper);

	assert(ext == L"DDS");

	Texture* texture = new Texture(imageFile);
	ID3D11Texture2D* srcTexture = texture->GetTexture();

	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, NULL, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);

	UINT* pixels = new UINT[readDesc.Width * readDesc.Height];

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);

	width = texture->GetWidth();
	height = texture->GetHeight();

	heights = new float[width * height];
	for (UINT i = 0; i < width * height; i++)
	{
		UINT temp = (pixels[i] & 0xFF000000) >> 24;
		heights[i] = (float)temp / 255.0f;
	}

	SafeDelete(texture);
	SafeRelease(readTexture);

	D3D11_TEXTURE2D_DESC heightDesc;
	ZeroMemory(&heightDesc, sizeof(D3D11_TEXTURE2D_DESC));
	heightDesc.Width = srcDesc.Width;
	heightDesc.Height = srcDesc.Height;
	heightDesc.ArraySize = 1;
	heightDesc.Format = DXGI_FORMAT_R32_FLOAT;
	heightDesc.MipLevels = 1;
	heightDesc.SampleDesc = srcDesc.SampleDesc;
	heightDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = heights;
	data.SysMemPitch = width * 4;
	data.SysMemSlicePitch = width * height * 4;

	Check(D3D::GetDevice()->CreateTexture2D(&heightDesc, &data, &heightMap));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Check(D3D::GetDevice()->CreateShaderResourceView(heightMap, &srvDesc, &heightMapSRV));

}

void TerrainLOD::CreatePatchVertex()
{
	vector<TerrainVertex> vertices(vertexCount);

	float halfWidth = GetWidth() * 0.5f;
	float halfHeight = GetHeight() * 0.5f;

	float tempWidth = GetWidth() / patchWidth;
	float tempHeight = GetHeight() / patchHeight;

	float du = 1.0f / patchWidth;
	float dv = 1.0f / patchHeight;

	for (UINT z = 0; z < patchHeight; z++)
	{
		float tempZ = halfHeight - z * tempHeight;

		for (UINT x = 0; x < patchWidth; x++)
		{
			float tempX = -halfWidth + x * tempWidth;

			vertices[patchWidth * z + x].Position = Vector3(tempX, 0.0f, tempZ);
			vertices[patchWidth * z + x].Uv.x = x * du;
			vertices[patchWidth * z + x].Uv.y = z * dv;
		}
	}

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(TerrainVertex));
}

void TerrainLOD::CreatePatchIndex()
{
	vector<UINT> indices(indexCount * 4);

	UINT index = 0;
	for (UINT z = 0; z < patchHeight - 1; z++)
	{
		for (UINT x = 0; x < patchWidth - 1; x++)
		{
			indices[index + 0] = patchWidth * z + x;
			indices[index + 1] = patchWidth * z + x + 1;
			indices[index + 2] = patchWidth * (z + 1) + x;
			indices[index + 3] = patchWidth * (z + 1) + x + 1;

			index += 4;
		}
	}

	indexBuffer = new IndexBuffer(&indices[0], indices.size());

}
