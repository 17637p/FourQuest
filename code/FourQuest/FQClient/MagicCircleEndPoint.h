#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class UVAnimator;
	class SoundEmitter;
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

	private:
		game_module::Transform* mTransform;
		game_module::UVAnimator* mUVAnimator;
		game_module::GameObject* mEndPointObject;
		game_module::SoundEmitter* mSoundEmitter;

		std::shared_ptr<game_module::GameObject> mDefenceCounterObjectOrNull;
		std::string mDefenceCounterObjectName;

		friend void RegisterMetaData();
	};
}


