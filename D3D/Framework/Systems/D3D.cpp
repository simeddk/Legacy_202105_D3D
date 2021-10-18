#include "Framework.h"
#include "D3D.h"

vector<D3DEnumAdapterInfo *> D3D::apapterInfos;
int D3D::selected_adapter_index = -1;

D3D* D3D::instance = nullptr;

D3DDesc D3D::d3dDesc;
ID3D11Device* D3D::device = nullptr;
ID3D11DeviceContext* D3D::deviceContext = nullptr;
IDXGISwapChain* D3D::swapChain = nullptr;

D3D * D3D::Get()
{
	//assert(instance != NULL);

	return instance;
}

void D3D::Create()
{
	assert(instance == nullptr);

	instance = new D3D();
}

void D3D::Delete()
{
	SafeDelete(instance);
}

void D3D::SetRenderTarget(ID3D11RenderTargetView * rtv, ID3D11DepthStencilView * dsv)
{
	if (rtv == nullptr)
		rtv = renderTargetView;

	if (dsv == nullptr)
		dsv = depthStencilView;

	D3D::GetDC()->OMSetRenderTargets(1, &rtv, dsv);
}

void D3D::Clear(D3DXCOLOR color, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
{
	if (rtv == nullptr)
		rtv = renderTargetView;

	if (dsv == nullptr)
		dsv = depthStencilView;

	deviceContext->ClearRenderTargetView(rtv, color);
	deviceContext->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void D3D::Present()
{
	swapChain->Present(d3dDesc.bVsync == true ? 1 : 0, 0);
}

void D3D::ResizeScreen(float width, float height)
{
	if (width < 1 || height < 1)
		return;

	d3dDesc.Width = width;
	d3dDesc.Height = height;

	DeleteBackBuffer();
	{
		HRESULT hr = swapChain->ResizeBuffers(0, (UINT)width, (UINT)height, DXGI_FORMAT_UNKNOWN, 0);
		assert(SUCCEEDED(hr));
	}
	CreateBackBuffer(width, height);
}

D3D::D3D()
	: numerator(0), denominator(1)
{
	SetGpuInfo();

	CreateSwapChainAndDevice();
	CreateBackBuffer(d3dDesc.Width, d3dDesc.Height);
}

D3D::~D3D()
{
	DeleteBackBuffer();

	if (swapChain != nullptr)
		swapChain->SetFullscreenState(false, NULL);

	SafeRelease(deviceContext);
	SafeRelease(device);
	SafeRelease(swapChain);
}

void D3D::EnumerateAdapters()
{
	IDXGIFactory1* factory;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void **)&factory)))
	{
		return;
	}

	UINT index = 0;
	while (true)
	{
		IDXGIAdapter1* adapter;
		HRESULT hr = factory->EnumAdapters1(index, &adapter);

		if (hr == DXGI_ERROR_NOT_FOUND)
			break;

		Check(hr);

		D3DEnumAdapterInfo* adapter_info = new D3DEnumAdapterInfo();
		ZeroMemory(adapter_info, sizeof(D3DEnumAdapterInfo));
		adapter_info->AdapterOrdinal = index;
		adapter->GetDesc1(&adapter_info->AdapterDesc);
		adapter_info->Adapter = adapter;

		EnumerateAdapterOutput(adapter_info);

		apapterInfos.push_back(adapter_info);

		++index;
	}

	SafeRelease(factory);
}

bool D3D::EnumerateAdapterOutput(D3DEnumAdapterInfo* adapter_info)
{
	HRESULT hr;
	IDXGIOutput* output = NULL;

	hr = adapter_info->Adapter->EnumOutputs(0, &output);
	if (DXGI_ERROR_NOT_FOUND == hr)
		return false;

	D3DEnumOutputInfo* output_info = new D3DEnumOutputInfo();
	ZeroMemory(output_info, sizeof(D3DEnumOutputInfo));

	hr = output->GetDesc(&output_info->OutputDesc);
	Check(hr);

	output_info->Output = output;

	UINT num_modes = 0;
	DXGI_MODE_DESC* display_modes = nullptr;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &num_modes, nullptr); // 모드 개수 가져오기
	Check(hr);

	display_modes = new DXGI_MODE_DESC[num_modes];
	hr = output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_modes); // 디스플레이 모드 가져오기
	Check(hr);


	for (UINT i = 0; i < num_modes; i++)
	{
		bool isCheck = true;
		isCheck &= display_modes[i].Width == d3dDesc.Width;
		isCheck &= display_modes[i].Height == d3dDesc.Height;

		if (isCheck == true)
		{
			output_info->Numerator = display_modes[i].RefreshRate.Numerator;
			output_info->Denominator = display_modes[i].RefreshRate.Denominator;
		}
	}

	adapter_info->OutputInfo = output_info;

	SafeDeleteArray(display_modes);

	return true;
}

void D3D::SetGpuInfo()
{
	EnumerateAdapters();
}

void D3D::CreateSwapChainAndDevice()
{
	SafeRelease(device);
	SafeRelease(deviceContext);
	SafeRelease(swapChain);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 0;
	swapChainDesc.BufferDesc.Height = 0;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (d3dDesc.bVsync == true)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = apapterInfos[0]->OutputInfo->Numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = apapterInfos[0]->OutputInfo->Denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = d3dDesc.Handle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = !d3dDesc.bFullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	creationFlags = D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif


	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
	};

	UINT maxVideoMemory = 0;
	for (UINT i = 0; i < apapterInfos.size(); ++i)
	{
		if (apapterInfos[i]->AdapterDesc.DedicatedVideoMemory > maxVideoMemory)
		{
			selected_adapter_index = i;
			maxVideoMemory = apapterInfos[i]->AdapterDesc.DedicatedVideoMemory; //더 메모리가 큰 장치는?
		}
	}

	numerator = apapterInfos[0]->OutputInfo->Numerator;
	denominator = apapterInfos[0]->OutputInfo->Denominator;

	gpuMemorySize = apapterInfos[selected_adapter_index]->AdapterDesc.DedicatedVideoMemory / 1024 / 1024;
	gpuDescription = apapterInfos[selected_adapter_index]->AdapterDesc.Description;

	HRESULT hr = D3D11CreateDeviceAndSwapChain
	(
		apapterInfos[selected_adapter_index]->Adapter
		, D3D_DRIVER_TYPE_UNKNOWN
		, NULL
		, creationFlags
		, featureLevels
		, 1
		, D3D11_SDK_VERSION
		, &swapChainDesc
		, &swapChain
		, &device
		, NULL
		, &deviceContext
	);
	assert(SUCCEEDED(hr));
}

void D3D::CreateBackBuffer(float width, float height)
{
	HRESULT hr;

	//Create RTV - System BackBuffer
	{
		ID3D11Texture2D* backbufferPointer;
		hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbufferPointer);
		Check(hr)

			hr = D3D::GetDevice()->CreateRenderTargetView(backbufferPointer, NULL, &renderTargetView);
		Check(hr)

			SafeRelease(backbufferPointer);
	}

	//Create Texture - DSV용
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &backBuffer);
		assert(SUCCEEDED(hr));
	}


	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		hr = D3D::GetDevice()->CreateDepthStencilView(backBuffer, &desc, &depthStencilView);
		assert(SUCCEEDED(hr));

		SetRenderTarget(renderTargetView, depthStencilView);
	}
}

void D3D::DeleteBackBuffer()
{
	SafeRelease(depthStencilView);
	SafeRelease(renderTargetView);
	SafeRelease(backBuffer);
}

D3DEnumAdapterInfo::D3DEnumAdapterInfo()
	: Adapter(nullptr), OutputInfo(nullptr)
{
}

D3DEnumAdapterInfo::~D3DEnumAdapterInfo()
{
	SafeRelease(Adapter);
	SafeDelete(OutputInfo);
}

D3DEnumOutputInfo::D3DEnumOutputInfo()
	: Output(nullptr), Numerator(0), Denominator(1)
{
}

D3DEnumOutputInfo::~D3DEnumOutputInfo()
{
	SafeRelease(Output);
}
