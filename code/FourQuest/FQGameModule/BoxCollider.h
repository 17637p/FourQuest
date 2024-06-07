#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class BoxCollider : public Component, public ICollider
	{
		using BoxInfo = fq::physics::BoxColliderInfo;
	public:
		BoxCollider();
		~BoxCollider();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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

		/// <summary>
		/// ���� ���� ����� ��ȯ�մϴ� 
		/// </summary>
		float GetStaticFriction()const;
		
		/// <summary>
		/// ���� ���� ����� �����մϴ�
		/// </summary>
		void SetStaticFriction(float friction);
		
		/// <summary>
		/// ���� ���� ����� ��ȯ�մϴ� 
		/// </summary>
		float GetDynamicFriction()const;

		/// <summary>
		/// ���� ���� ����� �����մϴ� 
		/// </summary>
		void SetDynamicFriction(float friction);

		/// <summary>
		/// ���� ����� ��ȯ�մϴ� 
		/// </summary>
		float GetRestituion()const;

		/// <summary>
		/// ���� ����� �����մϴ� 
		/// </summary>
		void SetRestituion(float restitution);

		/// <summary>
		/// �е��� �����մϴ�
		/// </summary>
		float GetDensity()const;
	
		/// <summary>
		/// �е��� ��ȯ�մϴ�
		/// </summary>
		void SetDensity(float density);

		/// <summary>
		/// ���� �浹���� �ݶ��̴��� ������ ��ȯ�մϴ� 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

		/// <summary>
		/// �ݶ��̴��� �������� ��ȯ�մϴ�.
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		
		/// <summary>
		/// �ݶ��̴��� �������� �����մϴ�.
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset)override { mOffset = offset; }
	
	private:
		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;

		entt::meta_handle GetHandle() override;

	private:
		fq::physics::EColliderType mType;
		DirectX::SimpleMath::Vector3 mOffset;
		BoxInfo mBoxinfomation;
		unsigned int mCollisionCount;
	};


}