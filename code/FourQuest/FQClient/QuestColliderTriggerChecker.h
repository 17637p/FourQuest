#pragma once
#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventHandler.h"

namespace fq::client
{
	class QuestColliderTriggerChecker : public game_module::Component
	{
	public:
		QuestColliderTriggerChecker();
		~QuestColliderTriggerChecker();

		QuestColliderTriggerChecker(const QuestColliderTriggerChecker& other);
		QuestColliderTriggerChecker& operator=(const QuestColliderTriggerChecker& other);

		virtual void OnTriggerEnter(const game_module::Collision& collision) override;
		virtual void OnTriggerStay(const game_module::Collision& collision) override;
		virtual void OnTriggerExit(const game_module::Collision& collision) override;

		virtual void OnAwake() override;
		void SetRequestPlayerNum(bool isAll, int requestPlayerNum);
		virtual void OnDestroy() override;

		void EventProcessUpdatePlayerState();
		int GetMaxPlayer();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;


	private:
		game_module::EventHandler mUpdatePlayerStateHandler;

		std::vector<bool> mIsAlive;

		int mCollidingPlayerNum;
		bool mIsClear;
		bool mIsAll;
		int mRequestPlayerNum; // isAll ¿Ã æ∆¥“∂ß
	};
}

