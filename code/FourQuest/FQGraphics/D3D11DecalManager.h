#pragma once

#include <set>
#include <memory>
#include <directxtk\SimpleMath.h>
#include <filesystem>
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class D3D11Device;
	class D3D11ResourceManager;

	class D3D11DecalManager
	{
	public:
		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		IDecalObject* CreateDecalObject(const DirectX::SimpleMath::Matrix& transform, const DecalInfo& decalInfo, std::shared_ptr<IDecalMaterial> iDecalMaterial);
		void DeleteDecalObject(IDecalObject* decalObject);

		const std::set<IDecalObject*> GetDecalObjects();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::set<IDecalObject*> mDecalObjects;
	};
}
