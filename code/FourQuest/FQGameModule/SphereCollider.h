#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class SphereCollider : public Component
	{
		using SphereColliderInfo = fq::physics::SphereColliderInfo;

	public:
		SphereCollider();
		~SphereCollider();

		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// �� �ݶ��̴��� ������ ��ȯ�մϴ�
		/// </summary>
		SphereColliderInfo GetSphereInfomation()const { return mSphereColliderInfo; }

		/// <summary>
		/// �� ���̴��� ������ �����մϴ�
		/// </summary>
		void SetSphereInfomation(SphereColliderInfo info) { mSphereColliderInfo = info; }

		/// <summary>
		/// �ݶ��̴� Ÿ���� ��ȯ�մϴ� 
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }

		/// <summary>
		/// �ݶ��̴� Ÿ���� �����մϴ� 
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }


		/// <summary>
		/// �������� ��ȯ�մϴ�
		/// </summary>
		float GetRadius()const { return mSphereColliderInfo.raidus; }

		/// <summary>
		/// �������� �����մϴ�
		/// </summary>
		void SetRadius(float radius);

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
		/// <returns></returns>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

	private:
		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;

		entt::meta_handle GetHandle() override;

	private:
		fq::physics::EColliderType mType;
		SphereColliderInfo mSphereColliderInfo;
		unsigned int mCollisionCount;
	};


}