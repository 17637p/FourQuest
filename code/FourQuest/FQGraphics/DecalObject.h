#pragma once

#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class Material;
	class D3D11ResourceManager;
	class IDecalMaterial;

	class DecalObject : public IDecalObject
	{
	public:
		DecalObject(const DirectX::SimpleMath::Matrix& transform, const DecalInfo& decalInfo, std::shared_ptr<IDecalMaterial> iDecalMaterial);
		virtual ~DecalObject() = default;

		// Info
		virtual void SetDecalInfo(const DecalInfo& decalInfo) override { mDecalInfo = decalInfo; }
		virtual const DecalInfo& GetDecalInfo() const override { return mDecalInfo; }

		// Transform
		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		// Material
		virtual void SetIDecalMaterial(std::shared_ptr<IDecalMaterial> iDecalMaterial) override { mIDecalMaterial = iDecalMaterial; }
		virtual std::shared_ptr<IDecalMaterial> GetDecalMaterial() const override { return mIDecalMaterial; }

	private:
		DirectX::SimpleMath::Matrix mTransform;
		DecalInfo mDecalInfo;
		std::shared_ptr<IDecalMaterial> mIDecalMaterial;
	};
}