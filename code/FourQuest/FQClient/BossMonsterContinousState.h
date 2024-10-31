#pragma once

#include "../FQGameModule/IStateBehaviour.h"

namespace fq::client
{
	class BossMonsterContinousState : public fq::game_module::IStateBehaviour
	{
		enum class EAttackType
		{
			First, // ù��° ����
			Second, // �ι�° ����
			InitReady, // �����ʱ�ȭ ���
		};

	public:
		BossMonsterContinousState();
		~BossMonsterContinousState();

	private:
		void OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		void OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt) override;
		void OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state) override;
		std::shared_ptr<IStateBehaviour> Clone() override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		std::shared_ptr<fq::game_module::GameObject> mDecalEfffect;

		float mAttackDuration;
		float mAttackElapsedTime;
		
		float mFirstHommingTime;
		float mFirstAttackTime;
		float mSecondHommingTime;
		float mSecondAttackTime;
		EAttackType mAttackType;
		float mPrevPlayBackSpeed;

		friend void RegisterMetaData();
	};


}