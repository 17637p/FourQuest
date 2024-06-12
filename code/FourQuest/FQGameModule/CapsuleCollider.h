#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class CapsuleCollider :public Component, public ICollider
	{
		using CapsuleInfo = fq::physics::CapsuleColliderInfo;

	public:
		enum class EDirection
		{
			XAxis,
			YAxis,
			ZAxis,
		};

	public:
		CapsuleCollider();
		~CapsuleCollider();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ĸ���� �������� �����մϴ�
		/// </summary>
		void SetRadius(float radius);

		/// <summary>
		/// ĸ���� �������� ��ȯ�մϴ� 
		/// </summary>
		float GetRadius()const;

		/// <summary>
		/// ĸ���� ������ ������ �����մϴ�
		/// </summary>
		void SetHalfHegiht(float halfHegiht);

		/// <summary>
		/// ĸ���� ������ ������ ��ȯ�մϴ� 
		/// </summary>
		float GetHalfHeight()const { return mCapsuleInfomation.halfHeight; }

		/// <summary>
		/// ĸ���� ������ ��ȯ�մϴ� 
		/// </summary>
		/// <returns></returns>
		CapsuleInfo GetCapsuleInfomation()const { return mCapsuleInfomation; }

		/// <summary>
		/// ĸ���� ������ �����մϴ� 
		/// </summary>
		void SetCapsuleInfomation(CapsuleInfo info) { mCapsuleInfomation = info; }

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
		/// �ݶ��̴� Ÿ���� ��ȯ�մϴ�
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }
		
		/// <summary>
		/// �ݶ��̴� Ÿ���� �����մϴ�
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }

		/// <summary>
		/// ���� �浹���� �ݶ��̴� ������ ��ȯ�մϴ� 
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

		/// <summary>
		/// ĸ���ݶ��̴� ������ ��ȯ�մϴ�.
		/// </summary>
		fq::game_module::CapsuleCollider::EDirection GetDirection() const { return mDirection; }
		
		/// <summary>
		/// ĸ���ݶ��̴� ������ �����մϴ�.
		/// </summary>
		void SetDirection(fq::game_module::CapsuleCollider::EDirection dir) { mDirection = dir; }


	private:
		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;

		entt::meta_handle GetHandle() override;

	private:
		physics::EColliderType mType;
		DirectX::SimpleMath::Vector3 mOffset;
		CapsuleInfo mCapsuleInfomation;
		EDirection mDirection;
		unsigned int mCollisionCount;
	};

}
