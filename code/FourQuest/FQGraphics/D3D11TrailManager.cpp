#include "D3D11TrailManager.h"
#include "ManagementCommon.h"
#include "Material.h"
#include "TrailObject.h"

namespace fq::graphics
{
	void D3D11TrailManager::Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
	}
	ITrailObject* D3D11TrailManager::CreateTrailObject(const TrailInfo& trailInfo)
	{
		ITrailObject* trailObjectInterface = new TrailObject(trailInfo, mResourceManager);

		mTrailObjects.insert(trailObjectInterface);

		return trailObjectInterface;
	}
	void D3D11TrailManager::DeleteTrailObject(ITrailObject* trailObjectInterface)
	{
		TrailObject* trailObject = static_cast<TrailObject*>(trailObjectInterface);
		delete trailObject;
		mTrailObjects.erase(trailObjectInterface);
	}
	const std::set<ITrailObject*> D3D11TrailManager::GetTrailObjects()
	{
		return mTrailObjects;
	}
}