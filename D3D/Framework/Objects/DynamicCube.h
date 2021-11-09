#pragma once

class DynamicCube
{
public:
	DynamicCube(Shader* shader, float width, float height);
	~DynamicCube();

	void PreRender(Vector3& position, Vector3& scale, float zNear = 0.1f, float zFar = 500.0f, float fov = 0.5f);
	void Render();

	ID3D11ShaderResourceView* SRV() { return  srv; }
	Projection* GetProjection() { return projection; }

	UINT& Type() { return desc.Type; }
	void Type(UINT val) { desc.Type = val; }

	float& Alpha() {return desc.Alpha; }
	float& RefractAmount() { return desc.RefractAmount; }

	float& FresnelAmount() { return desc.FresnelAmount; }
	float& FresnelBias() { return desc.FresnelBias; }
	float& FresnelScale() { return desc.FresnelScale; }

private:
	struct Desc
	{
		UINT Type = 0;
		float Alpha = 0.75f;
		float RefractAmount = 0.52f;
		float Padding;

		float FresnelAmount = 1.0f;
		float FresnelBias = 1.0f;
		float FresnelScale = 1.0f;
		float Padding2;

		Matrix Views[6];
		Matrix Projection;
	} desc;

private:
	Shader* shader;
	
	float width, height;

	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* rtvTexture;
	ID3D11ShaderResourceView* srv;

	ID3D11DepthStencilView* dsv;
	ID3D11Texture2D* dsvTexture;

	Viewport* viewport;

	Projection* projection;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	ID3DX11EffectShaderResourceVariable* sSrv;
};