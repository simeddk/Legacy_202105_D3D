#include "Framework.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(float width, float height, bool bUseStencil)
{
	this->width = (width < 1) ? D3D::Width() : width;
	this->height = (height < 1) ? D3D::Height() : height;

	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)this->width;
		desc.Height = (UINT)this->height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = bUseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &texture));
	}

	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = bUseStencil ? DXGI_FORMAT_D24_UNORM_S8_UINT : DXGI_FORMAT_D32_FLOAT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		Check(D3D::GetDevice()->CreateDepthStencilView(texture, &desc, &dsv));
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = bUseStencil ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : DXGI_FORMAT_R32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;
		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv));
	}
}

DepthStencil::~DepthStencil()
{
	SafeRelease(texture);
	SafeRelease(dsv);
	SafeRelease(srv);
}
