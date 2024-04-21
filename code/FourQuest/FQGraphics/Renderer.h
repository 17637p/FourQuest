//#pragma once
//
//#include <windows.h>
//#include <memory>
//
//namespace fq::graphics
//{
//	class D3D11Device;
//
//	class D3D11DepthStencilView;
//	class D3D11RenderTargetView;
//
//	class D3D11ResourceManager;
//
//	class Renderer
//	{
//	public:
//		Renderer();
//
//		void Initialize(const HWND hWnd, const unsigned short width, const unsigned short height);
//
//		void BeginRender();
//		void Render();
//		void EndRender();
//
//		void Finalize();
//
//		inline const std::shared_ptr<D3D11Device>& GetDevice() const;
//
//	private:
//		unsigned short mWindowWidth;
//		unsigned short mWindowHeight;
//
//		unsigned short mViewportWidth;
//		unsigned short mViewportHeight;
//
//	};
//
//}
//
