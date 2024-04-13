#pragma once

#include <wrl.h>
#include <d3d11.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

using namespace Microsoft::WRL;

namespace fq_graphics
{
	class D3D11Device
	{
	public:
		D3D11Device();

		bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height);

		ComPtr<ID3D11Device> GetDevice() const { return mDevice; }
		ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return mDeviceContext; }
		ComPtr<IDXGISwapChain> GetSwapChain() const { return mSwapChain; }

	private:
		ComPtr<ID3D11Device>			mDevice;
		ComPtr<ID3D11DeviceContext>		mDeviceContext;
		ComPtr<IDXGISwapChain>			mSwapChain;
	};
}

