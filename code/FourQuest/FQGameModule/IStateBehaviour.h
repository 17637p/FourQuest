#pragma once

#include "../FQReflect/FQReflect.h"

namespace fq::game_module
{
	class Animator;
	class AnimationStateNode;

	/// <summary>
	/// �ִϸ��̼� ���� �������̽� ��ũ��Ʈ 
	/// </summary>
	class IStateBehaviour : public fq::reflect::IHandle
	{
	public:
		virtual ~IStateBehaviour() {}

		/// <summary>
		/// ��ȯ�� ���۵ǰ� ���� �ӽ��� �Լ��� ȣ���մϴ� 
		/// </summary>
		virtual void OnStateEnter(Animator& animator, AnimationStateNode& state) abstract;
	
		/// <summary>
		/// OnStateEnter �� OnStateExit �ݹ� ������ �� ������Ʈ �����ӿ��� ȣ���մϴ�
		/// </summary>
		virtual void OnStateUpdate(Animator& animator, AnimationStateNode& state) abstract;
		
		/// <summary>
		/// Ʈ�������� ����ǰ� ���� �ӽ��� �Լ��� ȣ���մϴ�. 
		/// </summary>
		virtual void OnStateExit(Animator& animator, AnimationStateNode& state) abstract;
	};
}