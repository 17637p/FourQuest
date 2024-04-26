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
		/// 복사본을 반환합니다 
		/// </summary>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// 구의 반지름을 설정합니다
		/// </summary>
		void SetRadius(float radius);

		/// <summary>
		/// 구의 반지름을 반환합니다 
		/// </summary>
		float GetRadius()const;

		/// <summary>
		/// 캡슐의 정보를 반환합니다 
		/// </summary>
		/// <returns></returns>
		CapsuleInfo GetCapsuleInfomation()const { return mCapsuleInfomation; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		CapsuleInfo mCapsuleInfomation;
	};

}
