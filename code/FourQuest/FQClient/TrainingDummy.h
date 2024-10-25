#pragma once

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class Animator;
}

namespace fq::client
{
	class TrainingDummy : public fq::game_module::Component
	{
	public:
		TrainingDummy();
		~TrainingDummy();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnTriggerEnter(const game_module::Collision& collision);
		void OnStart() override;

	private:
		game_module::Animator* mAnimator;

		friend void RegisterMetaData();
	};
}