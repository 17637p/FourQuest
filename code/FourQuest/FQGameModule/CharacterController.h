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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ��� ������ ��ȯ�մϴ� 
		/// </summary>
		fq::physics::CharacterControllerInfo GetControllerInfo() const { return mControllerInfo; }
		
		/// <summary>
		/// ĳ���� ��Ʈ�ѷ��� ������ �����մϴ� 
		/// </summary>
		void SetControllerInfo(fq::physics::CharacterControllerInfo info);
	
		/// <summary>
		/// ĳ���� �̵� ������ ��ȯ�մϴ�
		/// </summary>
		fq::physics::CharacterMovementInfo GetMovementInfo() const { return mMovementInfo; }
		
		/// <summary>
		/// ĳ���� �̵� ������ �����մϴ� 
		/// </summary>
		void SetMovementInfo(fq::physics::CharacterMovementInfo val) { mMovementInfo = val; }
	
		/// <summary>
		/// ���� ĳ���Ͱ� ���������� ��ȯ�մϴ� 
		/// </summary>
		bool IsFalling() const { return mbIsFalling; }
	
		/// <summary>
		/// ���� ĳ���Ͱ� �������� �������� �����մϴ� 
		/// </summary>
		void SetFalling(bool val) { mbIsFalling = val; }

		/// <summary>
		/// �������� ��ȯ�մϴ� 
		/// </summary>
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		
		/// <summary>
		/// �������� �����մϴ� 
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset) override { mOffset = offset; }
	
		/// <summary>
		/// ��Ʈ�ѷ� ID�� ��ȯ�մϴ�.
		/// </summary>
		fq::game_module::CharacterController::ControllerID GetControllerID() const { return mControllerID; }
		
		/// <summary>
		/// ��Ʈ�ѷ� ID�� �����մϴ� ( 0 ~ 3)
		/// </summary>
		void SetControllerID(fq::game_module::CharacterController::ControllerID id);
	
		/// <summary>
		/// �̵� Ȱ��ȭ ���θ� ��ȯ�մϴ�
		/// </summary>
		bool OnMove() const { return mbOnMove; }
		
		/// <summary>
		/// �̵� Ȱ��ȭ ���θ� �����մϴ�
		/// </summary>
		void SetOnMove(bool val) { mbOnMove = val; }
		
		/// <summary>
		/// ȸ�� Ȱ��ȭ ���θ� ��ȯ�մϴ�
		/// </summary>
		bool OnRotation() const { return mbOnRotation; }
		
		/// <summary>
		/// ȸ�� Ȱ��ȭ ���θ� �����մϴ�
		/// </summary>
		void SetOnRotation(bool val) { mbOnRotation = val; }
	
		/// <summary>
		/// ���� ĳ���͸� �����ϼ� �ִ��� ��ȯ�մϴ�
		/// </summary>
		bool CanMoveCharater() const { return mbCanMoveCharater; }
		
		/// <summary>
		/// ���� ĳ���͸� ������ �� �ִ��� �����մϴ�.
		/// </summary>
		void SetCanMoveCharater(bool val) { mbCanMoveCharater = val; }

		/// <summary>
		/// ���� �浹���� �ݶ��̴� ������ ��ȯ�մϴ� 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

		/// <summary>
		/// ��Ʈ�ѷ� �Է¹����� �ٶ󺾴ϴ�
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
		std::array<bool, 4> mbMoveRestriction; // ĳ���� �̵����� ���� 
	};

}
