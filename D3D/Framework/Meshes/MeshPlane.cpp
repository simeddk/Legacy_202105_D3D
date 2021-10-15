#include "Framework.h"
#include "MeshPlane.h"

MeshPlane::MeshPlane(float offsetU, float offsetV)
	: offsetU(offsetU)
	, offsetV(offsetV)
{
}

MeshPlane::~MeshPlane()
{
}

void MeshPlane::Create()
{
	UINT countX = 11;
	UINT countZ = 11;

	float w = ((float)countX - 1) * 0.5f;
	float h = ((float)countZ - 1) * 0.5f;

	vector<VertexMesh> v;
	for (UINT z = 0; z < countZ; z++)
	{
		for (UINT x = 0; x < countX; x++)
		{
			VertexMesh vertex;
			vertex.Position = Vector3((float)x - w, 0, (float)z - h);
			vertex.Normal = Vector3(0, 1, 0);
			vertex.Uv.x = (float)x / (float)(countX - 1) * offsetU;
			vertex.Uv.y = (float)z / (float)(countZ - 1) * offsetV;

			v.push_back(vertex);
		}
	}

	vertices = new VertexMesh[v.size()];
	vertexCount = v.size();

	copy(v.begin(), v.end(), stdext::checked_array_iterator<VertexMesh*>(vertices, vertexCount));

	vector<UINT> i;
	for (UINT z = 0; z < countZ - 1; z++)
	{
		for (UINT x = 0; x < countX - 1; x++)
		{
			i.push_back(countX * z + x);
			i.push_back(countX * (z + 1) + x);
			i.push_back(countX * z + (x + 1));

			i.push_back(countX * z + (x + 1));
			i.push_back(countX * (z + 1) + x);
			i.push_back(countX * (z + 1) + (x + 1));
		}
	}

	indices = new UINT[i.size()];
	indexCount = i.size();

	copy(i.begin(), i.end(), stdext::checked_array_iterator<UINT*>(indices, indexCount));
}
