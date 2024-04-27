#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class BoxCollider : public Component
	{
		using BoxInfo = fq::physics::BoxColliderInfo;
	public:
		BoxCollider();
		~BoxCollider();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

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

	private:
		entt::meta_handle GetHandle() override;

	private:
		fq::physics::EColliderType mType;
		BoxInfo mBoxinfomation;
	};


}