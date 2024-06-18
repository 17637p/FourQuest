#pragma once

#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class Material;
	class D3D11ResourceManager;
	
	class DecalObject : public IDecalObject
	{
	public:
		DecalObject(std::shared_ptr<D3D11ResourceManager> resourceManager, const DecalInfo& decalInfo);
		virtual ~DecalObject() = default;

		virtual void SetDecalInfo(const DecalInfo& decalInfo) override;
		virtual const DecalInfo& GetDecalInfo() const override { return mDecalInfo; }

		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mDecalInfo.Transform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mDecalInfo.Transform; }

		std::shared_ptr<Material> GetMaterial() const { return mMaterial; }

	private:
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		DecalInfo mDecalInfo;
		std::shared_ptr<Material> mMaterial;
	};
}