#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// �ִϸ��̼��� �����ϴ� ������Ʈ
	/// </summary>
	class Animator :public Component
	{
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

	};

}
