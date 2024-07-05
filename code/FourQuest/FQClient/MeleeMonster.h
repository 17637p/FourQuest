#pragma once

#include "../FQGameModule/GameModule.h"
#include "IMonster.h"

namespace fq::client
{
	class GameManager;

	/// <summary>
	/// 근접몬스터
	/// </summary>
	class MeleeMonster : public game_module::Component , public IMonster
	{
	public:
		MeleeMonster();
		~MeleeMonster();

		void SetTarget(game_module::GameObject* target) override;

		void EmitAttack();

		void LookAtTarget();


	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;

	private:
		GameManager* mGameManager;
		fq::game_module::Animator* mAnimator;
		std::shared_ptr<game_module::GameObject> mTarget;

		float mMaxHp;
		float mHp;
		float mAttackPower;
		float mMoveSpeed;
		float mAcceleration;
		float mAttackRange;

		fq::game_module::PrefabResource mAttackPrefab;

		friend void RegisterMetaData();
	};


}
