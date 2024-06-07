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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 박스 콜라이더의 범위를 반환합니다 
		/// </summary>
		DirectX::SimpleMath::Vector3 GetExtent()const;
		
		/// <summary>
		/// 박스 콜라이더 범위를 설정합니다 
		/// </summary>
		void SetExtent(DirectX::SimpleMath::Vector3 extent);

		/// <summary>
		/// 박스콜라이더의 정보를 반환합니다
		/// </summary>
		BoxInfo GetBoxInfomation()const { return mBoxinfomation; }

		/// <summary>
		/// 콜라이더 타입을 반환합니다 
		/// </summary>
		fq::physics::EColliderType GetType() const { return mType; }

		/// <summary>
		/// 콜라이더 타입을 설정합니다 
		/// </summary>
		void SetType(fq::physics::EColliderType type) { mType = type; }
	
		/// <summary>
		/// 박스 콜라이더의 정보를 설정합니다
		/// </summary>
		void SetBoxInfomation(BoxInfo info) { mBoxinfomation = info; }

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
		/// 현재 충돌중이 콜라이더의 갯수를 반환합니다 
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