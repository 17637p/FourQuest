#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class BoxCollider : public Component
	{
		using BoxColliderInfo = fq::physics::BoxColliderInfo;
	public:
		BoxCollider();
		~BoxCollider();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// �ڽ� �ݶ��̴��� ������ ��ȯ�մϴ� 
		/// </summary>
		/// <returns>�ڽ� ����</returns>
		DirectX::SimpleMath::Vector3 GetExtent()const;
		
		/// <summary>
		/// �ڽ� �ݶ��̴� ������ �����մϴ� 
		/// </summary>
		void SetExtent(DirectX::SimpleMath::Vector3 extent);

		/// <summary>
		/// �ڽ��ݶ��̴��� ������ ��ȯ�մϴ�
		/// </summary>
		BoxColliderInfo GetBoxColliderInfomation()const { return mBoxinfo; }

	private:
		entt::meta_handle GetHandle() override;


	private:
		BoxColliderInfo mBoxinfo;
	};


}