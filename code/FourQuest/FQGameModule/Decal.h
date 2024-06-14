#pragma once
#include "Component.h"

#include <vector>
#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class Decal : public Component
	{
		using DecalInfo = graphics::DecalInfo;
		using DecalMaterialInfo = graphics::DecalMaterialInfo;

	public:
		Decal();
		~Decal() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetDecalInfo(const DecalInfo& decalInfo) { mDecalInfo = decalInfo; if (mDecalObjectInterface) { mDecalObjectInterface->SetDecalInfo(mDecalInfo); } }
		const DecalInfo& GetDecalInfo() const { return mDecalInfo; }

		void SetDecalMaterialInfo(const DecalMaterialInfo& decalMaterialInfo) { mDecalMaterialInfo = decalMaterialInfo; if (mMaterialInterface) { mMaterialInterface->SetDecalMaterialInfo(mDecalMaterialInfo); } }
		const DecalMaterialInfo& GetDecalMaterialInfo() const { return mDecalMaterialInfo; }

		void SetDecalObjectInterface(graphics::IDecalObject* decalObjectInterface) { mDecalObjectInterface = decalObjectInterface; }
		graphics::IDecalObject* GetDecalObjectInterface() const { return mDecalObjectInterface; }

		void SetIMaterial(std::shared_ptr<graphics::IMaterial> iMaterial) { mMaterialInterface = iMaterial; }
		std::shared_ptr<graphics::IMaterial> GetIMaterial() { return mMaterialInterface; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		graphics::IDecalObject* mDecalObjectInterface;
		std::shared_ptr<graphics::IMaterial> mMaterialInterface;
		DecalInfo mDecalInfo;
		DecalMaterialInfo mDecalMaterialInfo;
	};
}