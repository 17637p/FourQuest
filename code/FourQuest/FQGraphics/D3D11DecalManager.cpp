#include "D3D11DecalManager.h"
#include "ManagementCommon.h"
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
	void D3D11DecalManager::Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
	}
	IDecalObject* D3D11DecalManager::CreateDecalObject(const DecalInfo& decalInfo)
	{
		IDecalObject* decalObjectInterface = new DecalObject(mResourceManager, decalInfo);

		mDecalObjects.insert(decalObjectInterface);

		return decalObjectInterface;
	}
	void D3D11DecalManager::DeleteDecalObject(IDecalObject* decalObjectInterface)
	{
		DecalObject* decalObject = static_cast<DecalObject*>(decalObjectInterface);
		delete decalObject;
		mDecalObjects.erase(decalObjectInterface);
	}
	const std::set<IDecalObject*> D3D11DecalManager::GetDecalObjects()
	{
		return mDecalObjects;
	}
}