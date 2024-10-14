#pragma once

#include "Component.h"
#include "ICollider.h"

#include "../FQCommon/FQCommonPhysics.h"
#include "Transform.h"

namespace fq::game_module
{
	using ControllerID = unsigned int;

	class Controller : public Component, public ICollider
	{
	public:
		Controller();
		~Controller();


		fq::physics::CharacterControllerInfo GetControllerInfo() const { return mControllerInfo; }
		void SetControllerInfo(fq::physics::CharacterControllerInfo info);
		fq::physics::CharacterMovementInfo GetMovementInfo() const { return mMovementInfo; }
		void SetMovementInfo(fq::physics::CharacterMovementInfo val) { mMovementInfo = val; }
		DirectX::SimpleMath::Vector3 GetOffset() const override { return mOffset; }
		void SetOffset(DirectX::SimpleMath::Vector3 offset) override { mOffset = offset; }

		unsigned int GetControllerID() const { return mControllerID; }
		void SetControllerID(unsigned int id);
		bool IsFalling() const { return mbIsFalling; }
		void SetFalling(bool val) { mbIsFalling = val; }
		std::array<bool, 4> GetMoveRestrction()const { return mbMoveRestriction; }
		void SetMoveRestriction(std::array<bool, 4> restriction);

		/// <summary>
		/// ���� �浹���� �ݶ��̴� ������ ��ȯ�մϴ� 
		/// </summary>
		unsigned int GetCollisionCount() const { return mCollisionCount; }

	private:
		void OnStart() override;
		void OnFixedUpdate(float dt) override;

		void OnCollisionEnter(const Collision& collision) override;
		void OnCollisionExit(const Collision& collision) override;
		void OnTriggerEnter(const Collision& collision) override;
		void OnTriggerExit(const Collision& collision) override;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		entt::meta_handle GetHandle() override { return *this; }

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
		bool mbHasDashInput;

		std::array<bool, 4> mbMoveRestriction; // ĳ���� �̵����� ���� 
	};

}