#include "Framework.h"
#include "Viewport.h"

Viewport::Viewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}

void Viewport::Set(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.Width = this->width = width;
	viewport.Height = this->height = height;
	viewport.MinDepth = this->minDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	RSSetViewport();
}

Vector3 Viewport::Project(const Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;
	Vector3 result;

	Matrix wvp = W * V * P;
	D3DXVec3TransformCoord(&result, &position, &wvp);
	
	result.x = ((result.x + 1) * 0.5f) * width + x;
	result.y = ((-result.y + 1) * 0.5f) * height + y;
	result.z = (result.z * (maxDepth - minDepth)) + minDepth;

	return result;
}

Vector3 Viewport::Unproject(const Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;
	Vector3 result;

	result.x = ((position.x - x) / width) * 2.0f - 1.0f;
	result.y = (((position.y - y) / height) * 2.0f - 1.0f) * -1.0f;
	result.z = (position.z - minDepth) / (maxDepth - minDepth);

	Matrix wvp = W * V * P;
	D3DXMatrixInverse(&wvp, nullptr, &wvp);
	D3DXVec3TransformCoord(&result, &result, &wvp);

	return result;
}
