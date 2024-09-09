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

		unsigned int GetIsBlock() const { return mMaxBlockCount; }
		void SetIsBlock(unsigned int maxBlockCount) { mMaxBlockCount = maxBlockCount; }
		bool GetIsStrongAttack() const { return mbIsStrongAttack; }
		void SetIsStrongAttack(bool isStrongAttack) { mbIsStrongAttack = isStrongAttack; }
		float GetLifeTime() const { return mLifeTime; }
		void SetLifeTime(float time) { mLifeTime = time; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		// ������, ����� ���ݷ� �� �ִ� ���� ����
		bool mbIsStrongAttack;
		float mWeakAttackPower;
		float mStrongAttackPower;
		unsigned int mMaxBlockCount;

		// ���� �ð�
		float mLifeElapsedTime;
		float mLifeTime;

		// ������ �ϴ� ��ü 
		fq::game_module::GameObject* mAttacker;
		std::string mHitSound;
	};

}