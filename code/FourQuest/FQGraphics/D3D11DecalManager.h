#pragma once

#include <set>
#include <memory>
#include <directxtk\SimpleMath.h>
#include <filesystem>
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class Material;
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11ModelManager;

	class DecalObject : public IDecalObject
	{
	public:
		DecalObject(std::shared_ptr<D3D11ResourceManager> resourceManager, const DecalInfo& decalInfo);
		virtual ~DecalObject() = default;

		virtual void SetDecalInfo(const DecalInfo& decalInfo) override;
		virtual const DecalInfo& GetDecalInfo() const override { return mDecalInfo; }

		std::shared_ptr<Material> GetMaterial() const { return mMaterial; }

	private:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		DecalInfo mDecalInfo;
		std::shared_ptr<Material> mMaterial;
	};

	class D3D11DecalManager
	{
	public:
		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		IDecalObject* CreateDecalObject(const DecalInfo& decalInfo);
		void DeleteDecalObject(IDecalObject* decalObject);

		const std::set<IDecalObject*> GetDecalObjects();

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::set<IDecalObject*> mDecalObjects;
	};
}
