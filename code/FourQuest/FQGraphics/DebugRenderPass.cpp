#include "DebugRenderPass.h"

#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void DebugRenderPass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11JobManager> jobManager,
		std::shared_ptr<D3D11DebugDrawManager> debugDrawManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		unsigned short width,
		unsigned short height)
	{
		mDevice = device;
		mJobManager = jobManager;
		mDebugDrawManager = debugDrawManager;
		mCameraManager = cameraManager;
		mResourceManager = resourceManager;

		mBackBufferRTV = mResourceManager->Get<D3D11RenderTargetView>(ED3D11RenderTargetViewType::Offscreen);
		mDefaultDSV = mResourceManager->Get<D3D11DepthStencilView>(ED3D11DepthStencilViewType::Default);

		OnResize(width, height);
	}

	void DebugRenderPass::Finalize()
	{
		mDevice = nullptr;
		mJobManager = nullptr;
		mDebugDrawManager = nullptr;
		mCameraManager = nullptr;
		mResourceManager = nullptr;

		mBackBufferRTV = nullptr;
		mDefaultDSV = nullptr;
	}

	void DebugRenderPass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;
	}

	void DebugRenderPass::Render()
	{
		mBackBufferRTV->Bind(mDevice, mDefaultDSV);

		mDebugDrawManager->Excute(mDevice, mCameraManager);
	}
}