#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class CapsuleCollider :public Component
	{
		using CapsuleInfo = fq::physics::CapsuleColliderInfo;

	public:
		CapsuleCollider();
		~CapsuleCollider();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// ���� �������� �����մϴ�
		/// </summary>
		void SetRadius(float radius);

		/// <summary>
		/// ���� �������� ��ȯ�մϴ� 
		/// </summary>
		float GetRadius()const;

		/// <summary>
		/// ĸ���� ������ ��ȯ�մϴ� 
		/// </summary>
		/// <returns></returns>
		CapsuleInfo GetCapsuleInfomation()const { return mCapsuleInfomation; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		CapsuleInfo mCapsuleInfomation;
	};

}
