#include "D3D11Device.h"

#include <windows.h>
#include <cassert>

using namespace fq::graphics;

D3D11Device::D3D11Device()
	:mDevice(nullptr),
	mDeviceContext(nullptr),
	mSwapChain(nullptr),
	mWidth(0),
	mHeight(0)
{
}

bool D3D11Device::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
{
	mWidth = width;
	mHeight = height;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // D2D랑 연결하려면 플래그를 설정해야함
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// D3D를 어디(CPU or GPU)에서 실행할지 결정, 대부분 당연히 GPU에서 실행한다.
	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

	//Swap-Chain Setting
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SampleDesc.Count = 1;			// No MSAA
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		0, driverType, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &swapChainDesc,
		mSwapChain.GetAddressOf(), mDevice.GetAddressOf(), &featureLevel, mDeviceContext.GetAddressOf());

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	return true;
}

void D3D11Device::OnResize(const unsigned short width, const unsigned short height)
{
	mWidth = width;
	mHeight = height;
	HRESULT hr = mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	assert(SUCCEEDED(hr));
}
