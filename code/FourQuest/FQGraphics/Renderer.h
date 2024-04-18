#pragma once

#include <windows.h>
#include <memory>

namespace fq_graphics
{
	class D3D11Device;

	class D3D11DepthStencilView;
	class D3D11RenderTargetView;

	class D3D11ResourceManager;

	class Renderer
	{
	public:
		Renderer();

		void Initialize(const HWND hWnd, const unsigned short width, const unsigned short height);

		void BeginRender();
		void Render();
		void EndRender();

		void Finalize();

	private:
		unsigned short mWindowWidth;
		unsigned short mWindowHeight;

		unsigned short mViewportWidth;
		unsigned short mViewportHeight;

		std::shared_ptr<D3D11Device> mDevice;
		
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::shared_ptr<D3D11RenderTargetView> mRTVRenderer;
		std::shared_ptr<D3D11DepthStencilView> mDSV;
	};
}

