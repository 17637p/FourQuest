#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class BoxCollider : public Component
	{
		using BoxColliderInfo = fq::physics::BoxColliderInfo;
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
		/// <returns>박스 범위</returns>
		DirectX::SimpleMath::Vector3 GetExtent()const;
		
		/// <summary>
		/// 박스 콜라이더 범위를 설정합니다 
		/// </summary>
		void SetExtent(DirectX::SimpleMath::Vector3 extent);

		/// <summary>
		/// 박스콜라이더의 정보를 반환합니다
		/// </summary>
		BoxColliderInfo GetBoxColliderInfomation()const { return mBoxinfo; }

	private:
		entt::meta_handle GetHandle() override;


	private:
		BoxColliderInfo mBoxinfo;
	};


}