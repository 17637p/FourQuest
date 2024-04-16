#include "ResourceBase.h"

fq_graphics::ResourceBase::ResourceBase(const std::shared_ptr<D3D11ResourceManager> resourceManager, const ResourceType resourceType)
	:mResourceManager(resourceManager),
	mResourceType(resourceType)
{
}
