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
		/// 정지 마찰 계수를 반환합니다 
		/// </summary>
		float GetStaticFriction()const;

		/// <summary>
		/// 정지 마찰 계수를 설정합니다
		/// </summary>
		void SetStaticFriction(float friction);

		/// <summary>
		/// 동적 마찰 계수를 반환합니다 
		/// </summary>
		float GetDynamicFriction()const;

		/// <summary>
		/// 동적 마찰 계수를 설정합니다 
		/// </summary>
		void SetDynamicFriction(float friction);

		/// <summary>
		/// 복원 계수를 반환합니다 
		/// </summary>
		float GetRestituion()const;

		/// <summary>
		/// 복원 계수를 설정합니다 
		/// </summary>
		void SetRestituion(float restitution);

		/// <summary>
		/// 밀도를 설정합니다
		/// </summary>
		float GetDensity()const;

		/// <summary>
		/// 밀도를 반환합니다
		/// </summary>
		void SetDensity(float density);

		/// <summary>
		/// 트라이앵글 메쉬에 대한 정보를 반환합니다. 
		/// </summary>
		fq::physics::TriangleMeshColliderInfo GetTriangleMeshInfomation()const { return mTriangleMeshinfo; }

		/// <summary>
		/// 트라이앵글 메쉬에 대한 정보를 설정합니다.
		/// </summary>
		void SetTriangleMeshInfomation(fq::physics::TriangleMeshColliderInfo info) { mTriangleMeshinfo = info; }

		/// <summary>
		/// 콜라이더 타입을 반환합니다
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }

		/// <summary>
		/// 콜라이더 타입을 설정합니다
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }

		/// <summary>
		/// 현재 충돌중인 콜라이더 갯수를 반환합니다 
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