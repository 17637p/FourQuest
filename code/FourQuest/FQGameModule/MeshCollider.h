#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class MeshCollider : public Component , public ICollider
	{
		using ConvexMeshInfo = fq::physics::ConvexMeshColliderInfo;
	public:
		MeshCollider();
		~MeshCollider();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

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
		/// 컨벡스 메시에 생성할 버텍스 최대 갯수를 반환합니다 
		/// </summary>
		unsigned char GetConvexPolygonLimit()const;

		/// <summary>
		/// 컨벡스 메시에 생성할 버텍스 최대 갯수를 설정합니다 
		/// </summary>
		void SetConvexPolygonLimit(unsigned char limit);

		/// <summary>
		/// 컨벡스 메쉬에 대한 정보를 반환합니다 
		/// </summary>
		ConvexMeshInfo GetConvexMeshInfomation()const { return mConvexMeshInfomation; }

		/// <summary>
		/// 컨벡스 메쉬에 대한 정보를 설정합니다.
		/// </summary>
		void SetConvexMeshInfomation(ConvexMeshInfo info) { mConvexMeshInfomation = info; }

		/// <summary>
		/// 콜라이더의 오프셋을 반환합니다.
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		
		/// <summary>
		/// 콜라이더의 오프셋을 설정합니다.
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset) override { mOffset = offset; }

	private:
		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;

		entt::meta_handle GetHandle() override;
		
	private:
		physics::EColliderType mType;
		DirectX::SimpleMath::Vector3 mOffset;
		ConvexMeshInfo mConvexMeshInfomation;
		unsigned int mCollisionCount;
	};

}
