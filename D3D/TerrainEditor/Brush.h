#pragma once

class Brush
{
public:
	Brush(Shader* shader, Terrain* terrain);


private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);

		Vector3 Location;
		UINT Type = 0;

		UINT Range;
		float Padding[3];
	} brushDesc;

private:
	Shader* shader;
	Terrain* terrain;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

};