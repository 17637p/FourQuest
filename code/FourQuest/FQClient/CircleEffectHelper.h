#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class UVAnimator;
}

namespace fq::client
{
	class CircleEffectHelper : public game_module::Component
	{
	public:
		CircleEffectHelper() = default;
		~CircleEffectHelper() = default;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		game_module::Transform* mTransform;
		game_module::UVAnimator* mUVAnimator;
		game_module::GameObject* mEndPointObject;
		float mElapsed;

		// ������ ����
		float mDuration; // ������ ó�� �ð�
		std::string mPointName;

		friend void RegisterMetaData();
	};
}
