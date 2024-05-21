#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class CharacterController :	public Component
	{
	public:
		CharacterController();
		~CharacterController();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 캐릭터 컨트롤러의 정보를 반환합니다 
		/// </summary>
		fq::physics::CharacterControllerInfo GetControllerInfo() const { return mControllerInfo; }
		
		/// <summary>
		/// 캐릭터 컨트롤러의 정보를 설정합니다 
		/// </summary>
		void SetControllerInfo(fq::physics::CharacterControllerInfo val) { mControllerInfo = val; }
	
		/// <summary>
		/// 캐릭터 이동 정보를 반환합니다
		/// </summary>
		fq::physics::CharacterMovementInfo GetMovementInfo() const { return mMovementInfo; }
		
		/// <summary>
		/// 캐릭터 이동 정보를 설정합니다 
		/// </summary>
		void SetMovementInfo(fq::physics::CharacterMovementInfo val) { mMovementInfo = val; }
	
		/// <summary>
		/// 현재 캐릭터가 떨어지는지 반환합니다 
		/// </summary>
		bool IsFalling() const { return mbIsFalling; }
	
		/// <summary>
		/// 현재 캐릭터가 떨어지는 상태인지 설정합니다 
		/// </summary>
		void SetFalling(bool val) { mbIsFalling = val; }

		/// <summary>
		/// 오프셋을 반환합니다 
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const { return mOffset; }
		
		/// <summary>
		/// 오프셋을 설정합니다 
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset) { mOffset = offset; }
	
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		DirectX::SimpleMath::Vector3 mOffset;
		fq::physics::CharacterControllerInfo mControllerInfo;
		fq::physics::CharacterMovementInfo mMovementInfo;

		bool mbIsFalling;
	};

}
