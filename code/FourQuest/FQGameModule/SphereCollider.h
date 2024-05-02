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
		/// 구 콜라이더의 정보를 반환합니다
		/// </summary>
		SphereColliderInfo GetSphereInfomation()const { return mSphereColliderInfo; }

		/// <summary>
		/// 구 콜이더의 정보를 설정합니다
		/// </summary>
		void SetSphereInfomation(SphereColliderInfo info) { mSphereColliderInfo = info; }

		/// <summary>
		/// 콜라이더 타입을 반환합니다 
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }

		/// <summary>
		/// 콜라이더 타입을 설정합니다 
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }


		/// <summary>
		/// 반지름을 반환합니다
		/// </summary>
		float GetRadius()const { return mSphereColliderInfo.raidus; }

		/// <summary>
		/// 반지름을 설정합니다
		/// </summary>
		void SetRadius(float radius);

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
		/// 현재 충돌중인 콜라이더의 갯수를 반환합니다 
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