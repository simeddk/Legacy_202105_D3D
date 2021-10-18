#pragma once

class D3DEnumOutputInfo;
class D3DEnumAdapterInfo;

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

	static float Width()
	{
		return d3dDesc.Width;
	}

	static float Height()
	{
		return d3dDesc.Height;
	}

	static const D3DDesc& GetDesc()
	{
		return d3dDesc;
	}

	static const HWND& GetHandle()
	{
		return d3dDesc.Handle;
	}

	static void SetDesc(D3DDesc& desc)
	{
		d3dDesc = desc;
	}

	void SetRenderTarget(ID3D11RenderTargetView* rtv = nullptr, ID3D11DepthStencilView* dsv = nullptr);

	void Clear(D3DXCOLOR color = D3DXCOLOR(0xFF555566), ID3D11RenderTargetView* rtv = nullptr, ID3D11DepthStencilView* dsv = nullptr);
	void Present();

	void ResizeScreen(float width, float height);

	ID3D11DepthStencilView* DSV() { return depthStencilView; }

private:
	D3D();
	~D3D();

	static void EnumerateAdapters();
	static bool EnumerateAdapterOutput(D3DEnumAdapterInfo* adapter_info);

	static void SetGpuInfo();

	void CreateSwapChainAndDevice();

	void CreateBackBuffer(float width, float height);
	void DeleteBackBuffer();

private:
	static vector<D3DEnumAdapterInfo *> apapterInfos;
	static int selected_adapter_index;

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

	Vector2 WorldSize;
};

class D3DEnumAdapterInfo
{
	const D3DEnumAdapterInfo &operator = (const D3DEnumAdapterInfo &rhs);

public:
	D3DEnumAdapterInfo();
	~D3DEnumAdapterInfo();

	UINT AdapterOrdinal;
	IDXGIAdapter1* Adapter;
	DXGI_ADAPTER_DESC1 AdapterDesc;

	D3DEnumOutputInfo* OutputInfo;
};

class D3DEnumOutputInfo
{
	const D3DEnumOutputInfo &operator = (const D3DEnumOutputInfo &rhs);

public:
	D3DEnumOutputInfo();
	~D3DEnumOutputInfo();
	IDXGIOutput* Output;
	DXGI_OUTPUT_DESC OutputDesc;

	UINT Numerator;
	UINT Denominator;
};
