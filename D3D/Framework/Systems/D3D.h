#pragma once

struct D3DDesc
{
	wstring AppName;
	HINSTANCE Instance;
	HWND Handle;
	float Width;
	float Height;
	bool bVsync;
	bool bFullScreen;
	D3DXCOLOR Background;
};

class D3D
{
public:
	static D3D* Get();

	static void Create();
	static void Delete();

	static float Width() { return d3dDesc.Width; }
	static float Height() { return d3dDesc.Height; }


	static ID3D11Device* GetDevice()
	{
		return device;
	}

	static ID3D11DeviceContext* GetDC()
	{
		return deviceContext;
	}

	static IDXGISwapChain* GetSwapChain()
	{
		return swapChain;
	}

	static const D3DDesc& GetDesc()
	{
		return d3dDesc;
	}

	static void SetDesc(D3DDesc& desc)
	{
		d3dDesc = desc;
	}

	void SetRenderTarget(ID3D11RenderTargetView* rtv = NULL, ID3D11DepthStencilView* dsv = NULL);

	void Clear(D3DXCOLOR color = D3DXCOLOR(0xFF555566), ID3D11RenderTargetView* rtv = NULL, ID3D11DepthStencilView* dsv = NULL);
	void Present();

	void ResizeScreen(float width, float height);

private:
	D3D();
	~D3D();

	void SetGpuInfo();
	void CreateSwapChainAndDevice();

	void CreateBackBuffer(float width, float height);
	void DeleteBackBuffer();

private:
	static D3D* instance;

	static D3DDesc d3dDesc;
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
	static IDXGISwapChain* swapChain;

	ID3D11Debug* debugDevice;

	UINT gpuMemorySize;
	wstring gpuDescription;

	UINT numerator;
	UINT denominator;

	ID3D11Texture2D* backBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RenderTargetView* renderTargetView;
};