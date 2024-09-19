#include "ParticlePass.h"
#include "ManagementCommon.h"
#include "D3D11Common.h"

namespace fq::graphics
{
	void ParticlePass::Initialize(std::shared_ptr<D3D11Device> device,
		std::shared_ptr<D3D11ParticleManager> paricleManager,
		std::shared_ptr<D3D11ObjectManager> objectManager,
		std::shared_ptr<D3D11CameraManager> cameraManager,
		std::shared_ptr<D3D11ResourceManager> resourceManager,
		std::shared_ptr< D3D11LightManager> lightManager,
		unsigned short width,
		unsigned short height)
	{
		Finalize();

		mDevice = device;
		mParticleManager = paricleManager;
		mObjectManager = objectManager;
		mCameraManager = cameraManager;
		mLightManager = lightManager;
		mResourceManager = resourceManager;

		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		//OnResize(width, height);
		// rendering shader
	}
	void ParticlePass::Finalize()
	{
		mDevice = nullptr;
		mResourceManager = nullptr;
		mObjectManager = nullptr;
		mCameraManager = nullptr;
		mLightManager = nullptr;
		mParticleManager = nullptr;
	}

	void ParticlePass::OnResize(unsigned short width, unsigned short height)
	{
		mViewport.Width = (float)width;
		mViewport.Height = (float)height;
		mViewport.MinDepth = 0.f;
		mViewport.MaxDepth = 1.f;
		mViewport.TopLeftX = 0.f;
		mViewport.TopLeftY = 0.f;

		mParticleManager->OnResize(width, height);
	}

	void ParticlePass::Render()
	{
		mDevice->GetDeviceContext()->RSSetViewports(1, &mViewport);

		mParticleManager->BeginRender();

		for (const auto& particleObject : mObjectManager->GetParticleObjects())
		{
			mParticleManager->Render(particleObject);
		}

		mParticleManager->EndRender();
	}
}