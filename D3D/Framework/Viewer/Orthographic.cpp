#include "Framework.h"
#include "Orthographic.h"

Orthographic::Orthographic(float width, float height, float zn, float zf)
	: Projection(width, height, zn, zf, 0.0f)
{
	Set(width, height, zn, zf, 0.0f);
}

Orthographic::~Orthographic()
{
}

void Orthographic::Set(float width, float height, float zn, float zf, float fov)
{
	Super::Set(width, height, zn, zf, fov);

	D3DXMatrixOrthoLH(&matrix, width, height, zn, zf);
}
