#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class QuestColliderTriggerChecker : public game_module::Component
	{
	public:
		QuestColliderTriggerChecker();
		~QuestColliderTriggerChecker();

		virtual void OnTriggerEnter(const game_module::Collision& collision) override;
		virtual void OnTriggerExit(const game_module::Collision& collision) override;

		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mMaxPlayerNum;
		int mCollidingPlayerNum;
		bool mIsClear;
	};
}

