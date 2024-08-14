#pragma once

#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class TriangleCollider : public Component, public ICollider
	{
	public:
		TriangleCollider();
		~TriangleCollider();

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
		/// Ʈ���̾ޱ� �޽��� ���� ������ ��ȯ�մϴ�. 
		/// </summary>
		fq::physics::TriangleMeshColliderInfo GetTriangleMeshInfomation()const { return mTriangleMeshinfo; }

		/// <summary>
		/// Ʈ���̾ޱ� �޽��� ���� ������ �����մϴ�.
		/// </summary>
		void SetTriangleMeshInfomation(fq::physics::TriangleMeshColliderInfo info) { mTriangleMeshinfo = info; }

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

		virtual DirectX::SimpleMath::Vector3 GetOffset() const override;
		virtual void SetOffset(DirectX::SimpleMath::Vector3 offset) override;

	private:
		virtual entt::meta_handle GetHandle() override;
		virtual std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;

	private:
		physics::EColliderType					mType;
		DirectX::SimpleMath::Vector3			mOffset;
		fq::physics::TriangleMeshColliderInfo	mTriangleMeshinfo;
		unsigned int							mCollisionCount;
	};
}