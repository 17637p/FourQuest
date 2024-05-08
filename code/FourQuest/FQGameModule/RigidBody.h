#pragma once
#include "Component.h"

namespace fq::game_module
{
	class RigidBody : public Component
	{
	public:
		RigidBody();
		~RigidBody();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ�
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// true�� static body, false �� dynamic body �Դϴ�
		/// </summary>
		bool IsStatic() const { return mbIsStatic; }

		/// <summary>
		/// true�� static body, false �� dynamic body �Դϴ�
		/// </summary>
		void SetStatic(bool isStatic) { mbIsStatic = isStatic; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mbIsStatic;
	};


}