#pragma once

//-------------------------------------------------------------------
//ModelBone
//-------------------------------------------------------------------
class ModelBone
{
public:
	friend class Model;

private:
	ModelBone();
	~ModelBone();

public:
	int Index() { return index; }

	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }

	wstring Name() { return name; }

	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }

	vector<ModelBone*> Childs() { return childs; }

private:
	int index;
	wstring name;

	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	vector<ModelBone*> childs;
};

//-------------------------------------------------------------------
//ModelMesh
//-------------------------------------------------------------------
class ModelMesh
{
public:
	friend class Model;

private:
	ModelMesh();
	~ModelMesh();

private:
	struct BoneDesc
	{
		Matrix Transforms[MAX_MODEL_TRANSFORMS];

		UINT BoneIndex;
		float Padding[3];
	} boneDesc;

	//TODO ¸â¹ö ÇÔ¼ö¸¦ ¸¸µé Â÷·Ê

private:
	Shader* shader;

	Transform* trasnform = nullptr;
	PerFrame* perFrame = nullptr;

	int boneIndex;
	ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::VertexModel* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;

	vector<class ModelMeshPart*> meshParts;
};

//-------------------------------------------------------------------
//ModelMeshPart
//-------------------------------------------------------------------
class ModelMeshPart
{
public:
	friend class Model;
	friend class ModelMesh;
};