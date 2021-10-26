#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(wstring textureFile)
	: Renderer(L"23_Billboard.fxo")
{
	vertexCount = MAX_BILLBOARD_CONUT * 4;
	vertices = new VertexBillboard[vertexCount];
	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexBillboard), 0, true);

	indexCount = MAX_BILLBOARD_CONUT * 6;
	indices = new UINT[indexCount];

	for (UINT i = 0; i < MAX_BILLBOARD_CONUT; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}
	
	indexBuffer = new IndexBuffer(indices, indexCount);

	diffuseMap = new Texture(textureFile);
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Billboard::~Billboard()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeDelete(diffuseMap);
}

void Billboard::Render()
{
	if (prevCount != drawCount)
	{
		prevCount = drawCount;

		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, vertices, sizeof(VertexBillboard) * MAX_BILLBOARD_CONUT);
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);

	}

	Super::Render();

	sDiffuseMap->SetResource(diffuseMap->SRV());

	shader->DrawIndexed(0, Pass(), drawCount * 6);
}

void Billboard::Add(Vector3 & position, Vector2 & scale)
{
	vertices[drawCount * 4 + 0].Position = position;
	vertices[drawCount * 4 + 1].Position = position;
	vertices[drawCount * 4 + 2].Position = position;
	vertices[drawCount * 4 + 3].Position = position;

	vertices[drawCount * 4 + 0].Scale = scale;
	vertices[drawCount * 4 + 1].Scale = scale;
	vertices[drawCount * 4 + 2].Scale = scale;
	vertices[drawCount * 4 + 3].Scale = scale;

	vertices[drawCount * 4 + 0].Uv = Vector2(0, 1);
	vertices[drawCount * 4 + 1].Uv = Vector2(0, 0);
	vertices[drawCount * 4 + 2].Uv = Vector2(1, 1);
	vertices[drawCount * 4 + 3].Uv = Vector2(1, 0);

	drawCount++;
}
