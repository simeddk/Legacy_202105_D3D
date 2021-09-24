#include "stdafx.h"
#include "Converter.h"
#include "Types.h"
#include "Utilities/BinaryFile.h"

Converter::Converter()
{
	importer = new Assimp::Importer();
}

Converter::~Converter()
{
	SafeDelete(importer);
}

void Converter::ReadFile(wstring file)
{
	this->file = L"../../_Assets/" + file;

	scene = importer->ReadFile
	(
		String::ToString(this->file), 
		aiProcess_ConvertToLeftHanded |
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);


	if (scene == nullptr)
	{
		string str = importer->GetErrorString();
		MessageBoxA(D3D::GetDesc().Handle, str.c_str(), "Import Error", MB_OK);
		
		exit(-1);
	}

}

void Converter::ExportMesh(wstring savePath)
{
	ReadBoneData(scene->mRootNode, -1, -1);
	WriteMeshData(L"../../_Models/" + savePath + L".mesh");
}

void Converter::ReadBoneData(aiNode * node, int index, int parent)
{
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();
	
	Matrix transform(node->mTransformation[0]);
	D3DXMatrixTranspose(&bone->Transform, &transform);
	
	Matrix matParent;
	if (parent < 0)
		D3DXMatrixIdentity(&matParent);
	else
		matParent = bones[parent]->Transform;

	bone->Transform = bone->Transform * matParent;
	bones.push_back(bone);

	ReadMeshData(node, index);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Converter::ReadMeshData(aiNode * node, int index)
{
	if (node->mNumMeshes < 1) return;

	asMesh* mesh = new asMesh();
	mesh->BoneIndex = index;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		UINT startVeretx = mesh->Vertices.size();
		UINT startIndex = mesh->Indices.size();

		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			Model::VertexModel vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));

			if (srcMesh->HasTextureCoords(0))
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));

			if (srcMesh->HasNormals())
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));

			mesh->Vertices.push_back(vertex);
		}//for(v)

		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];

			for (UINT n = 0; n < face.mNumIndices; n++)
				mesh->Indices.push_back(face.mIndices[n] + startVeretx);
		}//for(f)

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];

		asMeshPart* meshPart = new asMeshPart();
		meshPart->MaterialName = material->GetName().C_Str();
		meshPart->StartVertex = startVeretx;
		meshPart->StartIndex = startIndex;
		meshPart->IndexCount = srcMesh->mNumFaces * srcMesh->mFaces->mNumIndices;

		mesh->MeshParts.push_back(meshPart);

	}//for(i)

	meshes.push_back(mesh);
}

void Converter::WriteMeshData(wstring savepath)
{
	Path::CreateFolders(Path::GetDirectoryName(savepath));

	BinaryWriter* w = new BinaryWriter(savepath);

	//Bone
	w->UInt(bones.size());
	for (asBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);
	}

	//Mesh
	w->UInt(meshes.size());
	for (asMesh* mesh : meshes)
	{
		w->Int(mesh->BoneIndex);

		w->UInt(mesh->Vertices.size());
		w->Byte(&mesh->Vertices[0], sizeof(Model::VertexModel) *  mesh->Vertices.size());

		w->UInt(mesh->Indices.size());
		w->Byte(&mesh->Indices[0], sizeof(UINT) * mesh->Indices.size());

		w->UInt(mesh->MeshParts.size());
		for (asMeshPart* part : mesh->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCont);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);
		}
	}

	SafeDelete(w);
}

void Converter::ExportMaterial(wstring savePath, bool bOverWrite)
{
	savePath = L"../../_Textures/" + savePath + L".material";

	if (bOverWrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}

	ReadMaterialData();
	WriteMaterialData(savePath);
}

void Converter::ReadMaterialData()
{
	for (UINT i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* srcMaterial = scene->mMaterials[i];
		if (FoundMaterialData(srcMaterial) == false)
			continue;

		asMaterial* material = new asMaterial();
		material->Name = srcMaterial->GetName().C_Str();

		aiColor3D color;

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->Ambient = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->Specular.a);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
		material->Emissive = Color(color.r, color.g, color.b, 1.0f);

		aiString file;
		srcMaterial->GetTexture(aiTextureType_AMBIENT, 0, &file);
		material->DiffuseFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

bool Converter::FoundMaterialData(aiMaterial * material)
{
	string materialName = material->GetName().C_Str();
	bool bFound = false;


	for (asMesh* mesh : meshes)
	{
		for (asMeshPart* part : mesh->MeshParts)
		{
			if (part->MaterialName == materialName)
				return true;
		}
	}

	return false;
}

void Converter::WriteMaterialData(wstring savePath)
{
}

string Converter::WriteTexture(string saveFolder, string file)
{
	return string();
}
