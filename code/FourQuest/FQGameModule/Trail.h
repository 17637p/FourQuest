#pragma once
#include "Component.h"

#include <vector>
#include "../FQCommon/IFQRenderObject.h"

namespace fq::game_module
{
	class Trail : public Component
	{
		using TrailInfo = graphics::TrailInfo;

	public:
		Trail();
		~Trail() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void SetTrailInfo(const TrailInfo& trailInfo) { mTrailInfo = trailInfo; if (mTrailObjectInterface) { mTrailObjectInterface->SetTrailInfo(mTrailInfo); } }
		const TrailInfo& GetTrailInfo() const { return mTrailInfo; }

		void SetTrailObjectInterface(graphics::ITrailObject* trailObjectInterface) { mTrailObjectInterface = trailObjectInterface; }
		graphics::ITrailObject* GetTrailObjectInterface() const { return mTrailObjectInterface; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		graphics::ITrailObject* mTrailObjectInterface;
		TrailInfo mTrailInfo;
	};
}