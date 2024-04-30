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

	private:
		entt::meta_handle GetHandle() override;

	private:
		physics::EColliderType mType;
		CapsuleInfo mCapsuleInfomation;
	};

}
