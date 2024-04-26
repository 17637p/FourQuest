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

		void OnResize(const unsigned short width, const unsigned short height);

		inline Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const;
		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() const;
		inline Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() const;

		inline unsigned short GetWidth() const;
		inline unsigned short GetHeight() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>			mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>		mDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain>			mSwapChain;

		unsigned short mWidth;
		unsigned short mHeight;
	};

	inline Microsoft::WRL::ComPtr<ID3D11Device> D3D11Device::GetDevice() const { return mDevice; }
	inline Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3D11Device::GetDeviceContext() const { return mDeviceContext; }
	inline Microsoft::WRL::ComPtr<IDXGISwapChain> D3D11Device::GetSwapChain() const { return mSwapChain; }

	inline unsigned short D3D11Device::GetWidth() const { return mWidth; }
	inline unsigned short D3D11Device::GetHeight() const { return mHeight; }
}

