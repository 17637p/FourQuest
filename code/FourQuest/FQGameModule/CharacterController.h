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
		/// ĳ���� �̵�ó���� �����մϴ� 
		/// </summary>
		void OnUpdate(float dt) override;

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
		DirectX::SimpleMath::Vector3 GetOffset() const { return mOffset; }
		
		/// <summary>
		/// �������� �����մϴ� 
		/// </summary>
		void SetOffset(DirectX::SimpleMath::Vector3 offset) { mOffset = offset; }
	
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
