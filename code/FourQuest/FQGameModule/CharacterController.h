#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class Transform;
	
	class CharacterController :	public Component
	{
		using ControllerID = unsigned int;

	public:
		CharacterController();
		~CharacterController();

		void OnStart() override;

		/// <summary>
		/// 캐릭터 이동처리를 수행합니다 
		/// </summary>
		void OnUpdate(float dt) override;

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
		void SetControllerInfo(fq::physics::CharacterControllerInfo info);
	
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
	
		/// <summary>
		/// 컨트롤러 ID를 반환합니다.
		/// </summary>
		fq::game_module::CharacterController::ControllerID GetControllerID() const { return mControllerID; }
		
		/// <summary>
		/// 컨트롤러 ID를 설정합니다 ( 0 ~ 3)
		/// </summary>
		void SetControllerID(fq::game_module::CharacterController::ControllerID id);
	
		/// <summary>
		/// 이동 활성화 여부를 반환합니다
		/// </summary>
		bool OnMove() const { return mbOnMove; }
		
		/// <summary>
		/// 이동 활성화 여부를 설정합니다
		/// </summary>
		void SetOnMove(bool val) { mbOnMove = val; }
		
		/// <summary>
		/// 회전 활성화 여부를 반환합니다
		/// </summary>
		bool OnRotation() const { return mbOnRotation; }
		
		/// <summary>
		/// 회전 활성화 여부를 설정합니다
		/// </summary>
		void SetOnRotation(bool val) { mbOnRotation = val; }
	
		/// <summary>
		/// 현재 캐릭터를 움직일수 있는지 반환합니다
		/// </summary>
		bool CanMoveCharater() const { return mbCanMoveCharater; }
		
		/// <summary>
		/// 현재 캐릭터를 움직일 수 있는지 설정합니다.
		/// </summary>
		void SetCanMoveCharater(bool val) { mbCanMoveCharater = val; }
	private:
		void getKeyInput(DirectX::SimpleMath::Vector3& input);

		entt::meta_handle GetHandle() override { return *this; }

	private:
		Transform* mTransform;

		DirectX::SimpleMath::Vector3 mOffset;
		fq::physics::CharacterControllerInfo mControllerInfo;
		fq::physics::CharacterMovementInfo mMovementInfo;

		ControllerID mControllerID;

		bool mbIsFalling;
		bool mbCanMoveCharater;
		bool mbOnMove;
		bool mbOnRotation;
	};

}
