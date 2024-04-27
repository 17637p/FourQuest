#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class BoxCollider : public Component
	{
		using BoxInfo = fq::physics::BoxColliderInfo;
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
		DirectX::SimpleMath::Vector3 GetExtent()const;
		
		/// <summary>
		/// �ڽ� �ݶ��̴� ������ �����մϴ� 
		/// </summary>
		void SetExtent(DirectX::SimpleMath::Vector3 extent);

		/// <summary>
		/// �ڽ��ݶ��̴��� ������ ��ȯ�մϴ�
		/// </summary>
		BoxInfo GetBoxInfomation()const { return mBoxinfomation; }

		/// <summary>
		/// �ݶ��̴� Ÿ���� ��ȯ�մϴ� 
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }

		/// <summary>
		/// �ݶ��̴� Ÿ���� �����մϴ� 
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }
	
		/// <summary>
		/// �ڽ� �ݶ��̴��� ������ �����մϴ�
		/// </summary>
		void SetBoxInfomation(BoxInfo info) { mBoxinfomation = info; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::physics::EColliderType mType;
		BoxInfo mBoxinfomation;
	};


}