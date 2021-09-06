#pragma once
#include "Framework.h"

//=======================================================================================

struct Vertex
{
	Vertex()
		: Position(0, 0, 0) {}

	Vector3 Position;
};

//=======================================================================================

struct VertexNormal
{
	VertexNormal()
		: Position(0, 0, 0)
		, Normal(0, 0, 0) {}

	VertexNormal(float x, float y, float z, float nx, float ny, float nz)
	{
		Position.x = x;
		Position.y = y;
		Position.z = z;

		Normal.x = nx;
		Normal.y = ny;
		Normal.z = nz;
	}

	Vector3 Position;
	Vector3 Normal;
};

//=======================================================================================

struct VertexColor
{
	VertexColor()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1) {}

	VertexColor(float x, float y, float z, float r, float g, float b)
	{
		Position.x = x;
		Position.y = y;
		Position.z = z;

		Color.r = r;
		Color.g = g;
		Color.b = b;
		Color.a = 1.0f;
	}

	Vector3	Position;
	Color Color;
};

//=======================================================================================

struct VertexColorNormal
{
	VertexColorNormal()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1)
		, Normal(0, 0, 0) {}

	Vector3 Position;
	Color Color;
	Vector3 Normal;
};

//=======================================================================================

struct VertexTexture
{
	VertexTexture()
		: Position(0, 0, 0)
		, Uv(0, 0) {}

	Vector3	Position;
	Vector2	Uv;
};

//=======================================================================================

struct VertexTextureColor
{
	VertexTextureColor()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Color(0, 0, 0, 1)
	{

	}

	Vector3 Position;
	Vector2 Uv;
	Color Color;
};

//=======================================================================================

struct VertexTextureColorNormal
{
	VertexTextureColorNormal()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Color(0, 0, 0, 1)
		, Normal(0, 0, 0)
	{

	}

	Vector3 Position;
	Vector2 Uv;
	Color Color;
	Vector3 Normal;
};

//=======================================================================================

struct VertexTextureNormal
{
	VertexTextureNormal()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0) {}

	VertexTextureNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz)
	{
		Position = Vector3(x, y, z);
		Uv = Vector2(u, v);
		Normal = Vector3(nx, ny, nz);
	}

	Vector3 Position;
	Vector2 Uv;
	Vector3 Normal;
};

//=======================================================================================

struct VertexColorTextureNormal
{
	VertexColorTextureNormal()
		: Position(0, 0, 0)
		, Color(0, 0, 0, 1)
		, Uv(0, 0)
		, Normal(0, 0, 0) {}

	Vector3 Position;
	Color Color;
	Vector2 Uv;
	Vector3 Normal;
};

//=======================================================================================

struct VertexTextureNormalBlend
{
	VertexTextureNormalBlend()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, BlendIndices(0, 0, 0, 0)
		, BlendWeights(0, 0, 0, 0) {}

	Vector3 Position;
	Vector2 Uv;
	Vector3 Normal;
	Vector4 BlendIndices;
	Vector4 BlendWeights;
};

//=======================================================================================

struct VertexTextureNormalTangent
{
	VertexTextureNormalTangent()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, Tangent(0, 0, 0)
	{}

	VertexTextureNormalTangent(float px, float py, float pz, float u, float v, float nx, float ny, float nz, float tx, float ty, float tz)
	{
		Position = Vector3(px, py, pz);
		Uv = Vector2(u, v);
		Normal = Vector3(nx, ny, nz);
		Tangent = Vector3(tx, ty, tz);
	}

	Vector3	Position;
	Vector2	Uv;
	Vector3	Normal;
	Vector3 Tangent;
};

//=======================================================================================

struct VertexTextureNormalTangentBlend
{
	VertexTextureNormalTangentBlend()
		: Position(0, 0, 0)
		, Uv(0, 0)
		, Normal(0, 0, 0)
		, Tangent(0, 0, 0)
		, BlendIndices(0, 0, 0, 0)
		, BlendWeights(0, 0, 0, 0)
	{}

	Vector3	Position;
	Vector2	Uv;
	Vector3	Normal;
	Vector3 Tangent;
	Vector4 BlendIndices;
	Vector4 BlendWeights;
};

//=======================================================================================