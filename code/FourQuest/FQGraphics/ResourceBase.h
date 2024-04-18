#pragma once

#include <memory>

namespace fq::graphics
{
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
		ResourceBase(const ResourceType resourceType);
		virtual ~ResourceBase() {};

		virtual ResourceType GetType() { return mResourceType; };

	private:
		ResourceType mResourceType;
	};
}

