#pragma once

#include <wrl.h>
#include <d3d11.h>

namespace fq::graphics
{
	class D3D11Device
	{
	public:
		D3D11Device();

		bool Initialize(const HWND hWnd, const unsigned short width, const unsigned short height);

		Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const { return mDevice; }
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return mDeviceContext; }
		Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() const { return mSwapChain; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>			mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		mDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>			mSwapChain;
	};
}

