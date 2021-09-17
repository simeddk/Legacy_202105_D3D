#pragma once

#include "stdafx.h"

struct asBone
{
	int Index;
	string Name;

	int Parent;
	Matrix Transform;
};


struct asMeshPart
{
	string MaterialName;

	UINT StartVertex;
	UINT VertexCont;
	UINT StartIndex;
	UINT IndexCount;
};

struct asMesh
{
	int BoneIndex;

	vector<Model::VertexModel> Vertices;
	vector<UINT> Indices;

	vector<asMeshPart*> MeshParts;
};