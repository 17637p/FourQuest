#include "DecalObject.h"
#include "Material.h"

namespace fq::graphics
{
	DecalObject::DecalObject(std::shared_ptr<D3D11ResourceManager> resourceManager, const DecalInfo& decalInfo)
		: mResourceManager(resourceManager)
		, mDecalInfo(decalInfo)
	{
		mMaterial = std::make_shared<Material>(mResourceManager, decalInfo.MatrialData, decalInfo.TextureBasePath);
	}
	void DecalObject::SetDecalInfo(const DecalInfo& decalInfo)
	{
		mDecalInfo = decalInfo;
		mMaterial->SetMaterialData(decalInfo.MatrialData);
		mMaterial->SetTextureBasePath(decalInfo.TextureBasePath);
	}
}