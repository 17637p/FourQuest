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
		/// 애니메이션 시스템 초기화
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// Scene의 애니메이션 상태를 업데이트 합니다 
		/// </summary>
		void UpdateAnimation(float dt);

		/// <summary>
		/// 애니메이션의 상태를 변경합니다 
		/// </summary>
		void ChangeAnimationState(const fq::event::ChangeAnimationState& event);

		/// <summary>
		/// 애니메이터 컨트롤러 로드
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