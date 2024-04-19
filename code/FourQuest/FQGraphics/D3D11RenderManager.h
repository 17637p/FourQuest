#pragma once

#include <memory>

#include <directxtk/SimpleMath.h>

#include "RenderJob.h"

namespace fq::graphics
{
	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11InputLayout;
	class D3D11VertexShader;
	class D3D11PixelShader;
	class D3D11Device;
	class D3D11SamplerState;

	class D3D11RenderManager
	{
	public:
		struct ModelTransfrom
		{
			DirectX::SimpleMath::Matrix WorldMat;
			DirectX::SimpleMath::Matrix WorldInvTransposeMat;
		};

		struct SceneTrnasform
		{
			DirectX::SimpleMath::Matrix ViewProjMat;
			DirectX::SimpleMath::Matrix ShadowViewProjTexMat;
		};

	public:
		D3D11RenderManager(const std::shared_ptr<D3D11Device>& device);

		void Render(const std::shared_ptr<D3D11Device>& device, const std::vector<StaticMeshJob>& staticMeshJobs);

	private:
		std::shared_ptr<D3D11InputLayout> mStaticMeshLayout;
		std::shared_ptr<D3D11VertexShader> mStaticMeshVS;
		std::shared_ptr<D3D11PixelShader> mStaticMeshPS;
		std::shared_ptr<D3D11SamplerState> mSamplerState;

		std::shared_ptr<D3D11ConstantBuffer<ModelTransfrom>> mModelTransformCB;
		std::shared_ptr<D3D11ConstantBuffer<SceneTrnasform>> mSceneTransformCB;
	};
}

