#include "D3D11DecalManager.h"
#include "ManagementCommon.h"
#include "Material.h"
#include "DecalObject.h"

namespace fq::graphics
{
	void D3D11DecalManager::Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
	}
	IDecalObject* D3D11DecalManager::CreateDecalObject(const DirectX::SimpleMath::Matrix& transform, const DecalInfo& decalInfo, std::shared_ptr<IDecalMaterial> iDecalMaterial)
	{
		IDecalObject* decalObjectInterface = new DecalObject(transform, decalInfo, iDecalMaterial);

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