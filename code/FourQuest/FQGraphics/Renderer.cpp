//#include "Renderer.h"
//
//#include "D3D11Device.h"
//#include "D3D11State.h"
//#include "D3D11View.h"
//#include "D3D11Buffer.h"
//
//#include "D3D11ResourceManager.h"
//#include "Define.h"
//
//using namespace fq::graphics;
//
//Renderer::Renderer()
//	:mWindowWidth(0),
//	mWindowHeight(0),
//	mViewportWidth(0),
//	mViewportHeight(0)
//{
//	mDevice = std::make_shared<D3D11Device>();
//}
//
//void Renderer::Initialize(const HWND hWnd, const unsigned short width, const unsigned short height)
//{
//	mViewportWidth = width;
//	mViewportHeight = height;
//
//	// 디바이스를 초기화 한다. (디바이스, 디바이스 컨텍스트)
//	mDevice->Initialize(hWnd, width, height);
//
//	mResourceManager = std::make_shared<D3D11ResourceManager>(mDevice);
//
//	struct Trasform
//	{
//		int a;
//		float b;
//		int c;
//		float d;
//	};
//
//	std::shared_ptr<D3D11ConstantBuffer<Trasform> > testConstantBuffer;
//
//	Trasform tt{};
//	tt.a = 1;
//	tt.b = 2;
//	tt.c = 3;
//	tt.d = 4;
//
//	testConstantBuffer = mResourceManager->Create<fq::graphics::D3D11ConstantBuffer<Trasform>>(ED3D11ConstantBuffer::Transform);
//	testConstantBuffer->Update(mDevice, tt);
//	testConstantBuffer->Bind(mDevice, ED3D11ShaderType::VertexShader, 0);
//}
//
//void Renderer::BeginRender()
//{
//	mRTVRenderer->Bind(mDevice, mResourceManager->Get<fq::graphics::D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default));
//
//	D3D11_VIEWPORT viewport;
//	viewport.Width = (float)mViewportWidth;
//	viewport.Height = (float)mViewportHeight;
//	viewport.MinDepth = 0.f;
//	viewport.MaxDepth = 1.f;
//	viewport.TopLeftX = 0.f;
//	viewport.TopLeftY = 0.f;
//	mDevice->GetDeviceContext()->RSSetViewports(1, &viewport);
//
//	mDSV->ClearDepth(mDevice);
//}
//
//void Renderer::Render()
//{
//	// 그린다.
//}
//
//void Renderer::EndRender()
//{
//	// present
//	ComPtr<IDXGISwapChain> swapChain = mDevice->GetSwapChain();
//	HR(swapChain->Present(0, 0));
//}
//
//void Renderer::Finalize()
//{
//
//}
//
//
//
