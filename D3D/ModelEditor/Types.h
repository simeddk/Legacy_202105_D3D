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

struct asMaterial
{
	string Name;

	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emissive;

	string DiffuseFile;
	string SpecularFile;
	string NormalFile;
};

struct asBlendWeight
{
	Vector4 Indices = Vector4(0, 0, 0, 0);
	Vector4 Weights = Vector4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float Weight)
	{
		float i = (float)boneIndex;
		float w = Weight;

		switch (index)
		{
			case 0 : Indices.x = i; Weights.x = w; break;
			case 1 : Indices.y = i; Weights.y = w; break;
			case 2 : Indices.z = i; Weights.z = w; break;
			case 3 : Indices.w = i; Weights.w = w; break;
		}
	}
};

struct asBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.0f) return;

		bool bAdd = false;
		
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeights));
				bAdd = true;

				break;
			}

			it++;
		}

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(asBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < BoneWeights.size(); i++)
		{
			if (i >= 4) return;

			blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
		}
	}


	void Normalize()
	{
		float totalWeights = 0.0f;
		int i = 0;

		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeights += it->second;
				i++; it++;
			}
			else
				it = BoneWeights.erase(it);
		}

		float ratio = 1.0f / totalWeights;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= ratio;
			it++;
		}
	}
};

//1 Bone, 1 Frame
struct asKeyFrameData
{
	float Frame;

	Vector3 Scale;
	Quaternion Rotation;
	Vector3 Translation;
};

//1 Bone, All Frame
struct asKeyFrame
{
	string BoneName;
	vector<asKeyFrameData> Transforms;
};

//All Bone, All Frame
struct asClip
{
	string Name;

	UINT FrameCount;
	float FrameRate;

	vector<asKeyFrame*> Keyframes;
};

//Áß°£
struct asClipNode
{
	aiString Name;
	vector<asKeyFrameData> KeyFrame;
};