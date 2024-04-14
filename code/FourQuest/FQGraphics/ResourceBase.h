#pragma once

#include <memory>

namespace fq_graphics
{
	class D3D11ResourceManager;

	enum class ResourceType
	{
		none,

		RenderTargetView,
		ShaderResourceView,
		DepthStencilView,

		SamplerState,
		RasterizerState,
		DepthStencilState,
		BlendState
	};

	class ResourceBase
	{
	public:
		ResourceBase(std::shared_ptr<D3D11ResourceManager> resourceManager, ResourceType resourceType);
		virtual ~ResourceBase() {};

		virtual ResourceType GetType() { return mResourceType; };

	protected:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

	private:
		ResourceType mResourceType;
	};
}