#pragma once

#include <set>
#include <memory>
#include <directxtk\SimpleMath.h>
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;
	class ITrailObject;

	class D3D11TrailManager
	{
	public:
		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		ITrailObject* CreateTrailObject(const TrailInfo& trailInfo);
		void DeleteTrailObject(ITrailObject* trailObject);

		const std::set<ITrailObject*> GetTrailObjects();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::set<ITrailObject*> mTrailObjects;
	};
}