#pragma once
#include "Component.h"

namespace fq::game_module
{
	class AnimatorController;

	/// <summary>
	/// �ִϸ��̼��� �����ϴ� ������Ʈ
	/// </summary>
	class Animator :public Component
	{
		using ControllerPath = std::string;

	public:
		Animator();
		~Animator();
	
		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		ControllerPath mPath;
		std::shared_ptr<AnimatorController> mController;

	};

}
