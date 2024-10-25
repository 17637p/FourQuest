#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class DefenceCounter : public game_module::Component
	{
	public:
		DefenceCounter();
		~DefenceCounter();

		virtual void OnTriggerEnter(const game_module::Collision& collision) override;
		virtual void OnTriggerExit(const game_module::Collision& collision) override;

		void SetIsStart(bool isStart) { mIsStart = isStart; }
		float GetCountRatio() { return mCurCount / mRequestCount; }

		virtual void OnAwake() override;
		virtual void OnUpdate(float dt) override;

	private:
		int getMaxPlayer();

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mCollidingPlayerNum;

		float mCurCount;
		float mRequestCount;

		float mCountSpeed;

		bool mIsStart;
		bool mIsClear;

		std::vector<bool> mIsAlive;

		friend void RegisterMetaData();
	};
}

