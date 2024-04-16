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

		Texture,

		SamplerState,
		RasterizerState,
		DepthStencilState,
		BlendState,

	};

	class ResourceBase
	{
	public:
		ResourceBase(const std::shared_ptr<D3D11ResourceManager> resourceManager, const ResourceType resourceType);
		virtual ~ResourceBase() {};

		virtual ResourceType GetType() { return mResourceType; };

	protected:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

	private:
		ResourceType mResourceType;
	};
}

