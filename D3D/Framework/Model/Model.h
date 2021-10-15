#pragma once

#define MAX_MODEL_TRANSFORMS 250
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

class ModelBone;
class ModelMesh;
class ModelMeshPart;
class ModelClip;

class Model
{
public:
	friend class ModelRender;
	friend class ModelAnimator;

public:
	struct VertexModel;

private:
	Model();
	~Model();

private:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);

private:
	void BindBone();
	void BindMesh();

public:
	UINT BoneCount() { return bones.size(); }
	vector<ModelBone*>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh*>& Meshes() { return meshes; }
	ModelMesh* MeshByInidex(UINT index) { return meshes[index]; }

	UINT MaterialCount() { return materials.size(); }
	vector<Material*>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	UINT ClipCount() { return clips.size(); }
	vector<ModelClip*>& Clips() { return clips; }
	ModelClip* ClipByIndex(UINT index) { return clips[index]; }
	ModelClip* ClipByName(wstring name);

public:
	struct VertexModel
	{
		Vector3 Position;
		Vector2 Uv;
		Vector3 Normal;
		Vector3 Tangent;
		Vector4 BlendIndices;
		Vector4 BlendWeights;

		VertexModel()
		{
			Position = Vector3(0, 0, 0);
			Uv = Vector2(0, 0);
			Normal = Vector3(0, 0, 0);
			Tangent = Vector3(0, 0, 0);
			BlendIndices = Vector4(0, 0, 0, 0);
			BlendWeights = Vector4(0, 0, 0, 0);
		}
	};

private:
	ModelBone* root;

	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;
	vector<Material*> materials;
	vector<ModelClip*> clips;
};