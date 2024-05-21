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
		void SetControllerInfo(fq::physics::CharacterControllerInfo val) { mControllerInfo = val; }
	
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
	
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		DirectX::SimpleMath::Vector3 mOffset;
		fq::physics::CharacterControllerInfo mControllerInfo;
		fq::physics::CharacterMovementInfo mMovementInfo;

		bool mbIsFalling;
	};

}
