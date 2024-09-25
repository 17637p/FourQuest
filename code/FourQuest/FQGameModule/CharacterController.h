#pragma once
#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	enum class EPadStickType
	{
		Left,
		Right,
	};

	class Transform;
	
	class CharacterController :	public Component, public ICollider
	{
		using ControllerID = unsigned int;

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
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		
		/// <summary>
		/// 오프셋을 설정합니다 
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset) override { mOffset = offset; }
	
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

		/// <summary>
		/// 현재 충돌중인 콜라이더 갯수를 반환합니다 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

		/// <summary>
		/// 컨트롤러 입력방향을 바라봅니다
		/// </summary>
		void SetPadInputRotation(EPadStickType padStickType = EPadStickType::Left);

		float GetFinalSpeedMultiplier() { return mFinalSpeedMultiplier; }
		void AddFinalSpeedMultiplier(float value) { mFinalSpeedMultiplier += value; }
		float GetBaseSpeed() { return mBaseSpeed; }

		void SetDashInput(bool val) { mbHasDashInput = val; }

		std::array<bool, 4> GetMoveRestrction()const { return mbMoveRestriction; }

		void SetMoveRestriction(std::array<bool, 4> restriction);

	private:
		void OnStart() override;
		void OnUpdate(float dt) override;
		void OnFixedUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }
		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;
	private:
		Transform* mTransform;

		DirectX::SimpleMath::Vector3 mOffset;
		fq::physics::CharacterControllerInfo mControllerInfo;
		fq::physics::CharacterMovementInfo mMovementInfo;

		ControllerID mControllerID;
		unsigned int mCollisionCount;

		bool mbIsFalling;
		bool mbOnMove;
		bool mbCanMoveCharater;
		bool mbOnRotation;
		bool mbHasInput;
		bool mbHasDashInput;
		float mBaseSpeed;
		float mBaseAcceleration;
		float mFinalSpeedMultiplier;
		std::array<bool, 4> mbMoveRestriction; // 캐릭터 이동방향 제한 
	};

}
