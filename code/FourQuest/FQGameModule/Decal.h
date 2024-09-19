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

		void SetDecalObjectInterface(graphics::IDecalObject* decalObjectInterface) { mDecalObjectInterface = decalObjectInterface; }
		graphics::IDecalObject* GetDecalObjectInterface() const { return mDecalObjectInterface; }

		void SetDecalMaterialInfo(const DecalMaterialInfo& decalMaterialInfo) { mDacalMaterialInfo = decalMaterialInfo; if (mDecalMaterial != nullptr) { mDecalMaterial->SetInfo(mDacalMaterialInfo); } }
		const DecalMaterialInfo& GetDecalMaterialInfo() const { return mDacalMaterialInfo; }

		void SetDecalMaterial(std::shared_ptr<graphics::IDecalMaterial> decalMaterial) { mDecalMaterial = decalMaterial; }
		std::shared_ptr<graphics::IDecalMaterial> GetDecalMaterial() const { return mDecalMaterial; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		graphics::IDecalObject* mDecalObjectInterface;
		std::shared_ptr<graphics::IDecalMaterial> mDecalMaterial;
		DecalInfo mDecalInfo;
		DecalMaterialInfo mDacalMaterialInfo;
	};
}