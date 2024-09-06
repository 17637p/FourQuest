#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	class ArrowAttack : public game_module::Component
	{
	public:
		ArrowAttack();
		~ArrowAttack();

		virtual void OnFixedUpdate(float dt) override;

		virtual void OnTriggerEnter(const game_module::Collision& collision) override;
		virtual void OnTriggerStay(const game_module::Collision& collision) override;
		virtual void OnTriggerExit(const game_module::Collision& collision) override;

		bool GetIsBlock() const { return mbIsBlock; }
		void SetIsBlock(bool isBlock) { mbIsBlock = isBlock; }
		bool GetIsStrongAttack() const { return mbIsStrongAttack; }
		void SetIsStrongAttack(bool isStrongAttack) { mbIsStrongAttack = isStrongAttack; }
		float GetLifeTime() const { return mLifeTime; }
		void SetLifeTime(float time) { mLifeTime = time; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		bool mbIsBlock;
		bool mbIsStrongAttack;

		float mDurationTime;
		float mLifeTime;
	};

}