#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class UVAnimator;
}

namespace fq::client
{
	class MagicCircleEndPoint : public game_module::Component
	{
	public:
		MagicCircleEndPoint() = default;
		~MagicCircleEndPoint() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

		void OnTriggerEnter(const game_module::Collision& collision);
		void OnTriggerExit(const game_module::Collision& collision);

	private:
		game_module::Transform* mTransform;
		game_module::UVAnimator* mUVAnimator;
		game_module::GameObject* mEndPointObject;
		float mDuration; // 게이지 처리 시간

		float mElapsed;
		int mPlayerCount;
		bool mbIsStart;

		friend void RegisterMetaData();
	};
}


