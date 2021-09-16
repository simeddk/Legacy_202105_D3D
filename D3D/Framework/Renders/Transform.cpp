#include "Framework.h"
#include "Transform.h"

Transform::Transform()
	: shader(nullptr)
	, buffer(nullptr)
	, position(0, 0, 0)
	, scale(1, 1, 1)
	, rotation(0, 0, 0)
{
	D3DXMatrixIdentity(&bufferDesc.World);
}

Transform::Transform(Shader * shader)
	: position(0, 0, 0)
	, scale(1, 1, 1)
	, rotation(0, 0, 0)
{
	SetShader(shader);

	D3DXMatrixIdentity(&bufferDesc.World);
}

Transform::~Transform()
{
	SafeDelete(buffer);
}

void Transform::Set(Transform * trnasform)
{
	position = trnasform->position;
	scale = trnasform->scale;
	rotation = trnasform->rotation;

	UpdateWorld();
}

void Transform::SetShader(Shader * shader)
{
	this->shader = shader;

	buffer = new ConstantBuffer(&bufferDesc, sizeof(BufferDesc));
	sBuffer = shader->AsConstantBuffer("CB_World");
}

void Transform::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Transform::Position(Vector3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Transform::Position(Vector3 * vec)
{
	*vec = position;
}

void Transform::Scale(float x, float y, float z)
{
	Scale(Vector3(x, y, z));
}

void Transform::Scale(Vector3 & vec)
{
	scale = vec;
	UpdateWorld();
}

void Transform::Scale(Vector3 * vec)
{
	*vec = scale;
}

void Transform::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Transform::Rotation(Vector3 & vec)
{
	rotation = vec;
	UpdateWorld();
}

void Transform::Rotation(Vector3 * vec)
{
	*vec = rotation;
}

void Transform::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Transform::RotationDegree(Vector3 & vec)
{
	Vector3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void Transform::RotationDegree(Vector3 * vec)
{
	Vector3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	*vec = temp;
}

Vector3 Transform::Forward()
{
	return Vector3(bufferDesc.World._31, bufferDesc.World._32, bufferDesc.World._33);
}

Vector3 Transform::Up()
{
	return Vector3(bufferDesc.World._21, bufferDesc.World._22, bufferDesc.World._23);
}

Vector3 Transform::Right()
{
	return Vector3(bufferDesc.World._11, bufferDesc.World._12, bufferDesc.World._13);
}

void Transform::World(Matrix & matrix)
{
	Math::MatrixDecompose(matrix, scale, rotation, position);

	bufferDesc.World = matrix;
}

void Transform::UpdateWorld()
{
	Matrix S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	Matrix world = S * R * T;

	bufferDesc.World = world;
}

void Transform::Update()
{
}

void Transform::Render()
{
	if (shader == nullptr)  return;

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());
}
