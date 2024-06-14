#pragma once
#include "Component.h"

#include <vector>
#include "../FQCommon/IFQRenderObject.h"

namespace fq::game_module
{
	class Decal : public Component
	{
		using DecalInfo = graphics::DecalInfo;

	public:
		Decal();
		~Decal() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetDecalInfo(const DecalInfo& decalInfo) { mDecalInfo = decalInfo; if (mDecalObjectInterface) { mDecalObjectInterface->SetDecalInfo(mDecalInfo); } }
		const DecalInfo& GetDecalInfo() const { return mDecalInfo; }

		void SetDecalObjectInterface(graphics::IDecalObject* decalObjectInterface) { mDecalObjectInterface = decalObjectInterface; }
		graphics::IDecalObject* GetDecalObjectInterface() const { return mDecalObjectInterface; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		graphics::IDecalObject* mDecalObjectInterface;
		DecalInfo mDecalInfo;
	};
}