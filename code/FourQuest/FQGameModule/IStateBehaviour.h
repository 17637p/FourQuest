#pragma once

#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class Animator;
	class AnimationStateNode;

	/// <summary>
	/// 애니메이션 상태 인터페이스 스크립트 
	/// </summary>
	class IStateBehaviour : public fq::reflect::IHandle
	{
	public:
		virtual ~IStateBehaviour() {}

		/// <summary>
		/// 전환이 시작되고 상태 머신이 함수를 호출합니다 
		/// </summary>
		virtual void OnStateEnter(Animator& animator, AnimationStateNode& state) abstract;
	
		/// <summary>
		/// OnStateEnter 와 OnStateExit 콜백 사이의 각 업데이트 프레임에서 호출합니다
		/// </summary>
		virtual void OnStateUpdate(Animator& animator, AnimationStateNode& state) abstract;
		
		/// <summary>
		/// 트랜지션이 종료되고 상태 머신이 함수를 호출합니다. 
		/// </summary>
		virtual void OnStateExit(Animator& animator, AnimationStateNode& state) abstract;
	};
}