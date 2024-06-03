#pragma once

#include "../FQGameModule/GameModule.h"
#include "../FQGraphics/IFQGraphics.h"

namespace fq::game_engine
{
	class GameProcess;

	class AnimationSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		AnimationSystem();
		~AnimationSystem();

		/// <summary>
		/// �ִϸ��̼� �ý��� �ʱ�ȭ
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// Scene�� �ִϸ��̼� ���¸� ������Ʈ �մϴ� 
		/// </summary>
		void UpdateAnimation(float dt);

		/// <summary>
		/// �ִϸ��̼��� ���¸� �����մϴ� 
		/// </summary>
		void ChangeAnimationState(const fq::event::ChangeAnimationState& event);

		/// <summary>
		/// �ִϸ����� ��Ʈ�ѷ� �ε�
		/// </summary>
		/// <param name="object"></param>
		bool LoadAnimatorController(fq::game_module::GameObject* object);

	private:
		void updateAnimtorState(float dt);
		void processAnimation(float dt);


	public:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene; 
		fq::graphics::IFQGraphics* mGraphics;
		fq::game_module::AnimatorControllerLoader mLoader;

		EventHandler mChangeAnimationStateHandler;
	};


}