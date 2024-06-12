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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 캡슐의 반지름을 설정합니다
		/// </summary>
		void SetRadius(float radius);

		/// <summary>
		/// 캡슐의 반지름을 반환합니다 
		/// </summary>
		float GetRadius()const;

		/// <summary>
		/// 캡슐의 높이의 절반을 설정합니다
		/// </summary>
		void SetHalfHegiht(float halfHegiht);

		/// <summary>
		/// 캡슐의 높이의 절반을 반환합니다 
		/// </summary>
		float GetHalfHeight()const { return mCapsuleInfomation.halfHeight; }

		/// <summary>
		/// 캡슐의 정보를 반환합니다 
		/// </summary>
		/// <returns></returns>
		CapsuleInfo GetCapsuleInfomation()const { return mCapsuleInfomation; }

		/// <summary>
		/// 캡슐의 정보를 설정합니다 
		/// </summary>
		void SetCapsuleInfomation(CapsuleInfo info) { mCapsuleInfomation = info; }

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

		/// <summary>
		/// 콜라이더의 오프셋을 반환합니다.
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }

		/// <summary>
		/// 콜라이더의 오프셋을 설정합니다.
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset)override { mOffset = offset; }

		/// <summary>
		/// 캡슐콜라이더 방향을 반환합니다.
		/// </summary>
		fq::game_module::CapsuleCollider::EDirection GetDirection() const { return mDirection; }
		
		/// <summary>
		/// 캡슐콜라이더 방향을 설정합니다.
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
