#pragma once
#include "Component.h"

namespace fq::game_module
{
	class RigidBody : public Component
	{
		using Vector3 = DirectX::SimpleMath::Vector3;
	public:
		enum class EBodyType
		{
			Static,
			Dynamic,
			Kinematic,
		};

	public:
		RigidBody();
		~RigidBody();

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ���ӵ��� ��ȯ�մϴ� 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetLinearVelocity() const { return mLinearVelocity; }

		/// <summary>
		/// ���ӵ��� ���մϴ�.
		/// </summary>
		void AddLinearVelocity(Vector3 velocity);

		/// <summary>
		/// ���ӵ��� �����մϴ� 
		/// </summary>
		void SetLinearVelocity(fq::game_module::RigidBody::Vector3 val) { mLinearVelocity = val; }
		
		/// <summary>
		/// ���ӵ��� ��ȯ�մϴ� 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetAngularVelocity() const { return mAngularVelocity; }
		
		/// <summary>
		/// ���ӵ��� �����մϴ� 
		/// </summary>
		void SetAngularVelocity(fq::game_module::RigidBody::Vector3 val) { mAngularVelocity = val; }

		/// <summary>
		/// ������ �ٵ� Ÿ���� ��ȯ�մϴ�
		/// </summary>
		fq::game_module::RigidBody::EBodyType GetBodyType() const { return mBodyType; }
		
		/// <summary>
		/// ������ �ٵ� Ÿ���� �����մϴ�
		/// </summary>
		void SetBodyType(fq::game_module::RigidBody::EBodyType val) { mBodyType = val; }

		/// <summary>
		/// ��� ȸ�� �� �̵� ����� �����մϴ�.
		/// </summary>
		void SetIsLockLinearX(bool isLockLinearX) { mbIsLockLinearX = isLockLinearX; }
		void SetIsLockLinearY(bool isLockLinearY) { mbIsLockLinearY = isLockLinearY; }
		void SetIsLockLinearZ(bool isLockLinearZ) { mbIsLockLinearZ = isLockLinearZ; }
		void SetIsLockAngularX(bool isLockAngularX) { mbIsLockAngularX = isLockAngularX; }
		void SetIsLockAngularY(bool isLockAngularY) { mbIsLockAngularY = isLockAngularY; }
		void SetIsLockAngularZ(bool isLockAngularZ) { mbIsLockAngularZ = isLockAngularZ; }
		bool GetIsLockLinearX() const { return mbIsLockLinearX; }
		bool GetIsLockLinearY() const { return mbIsLockLinearY; }
		bool GetIsLockLinearZ() const { return mbIsLockLinearZ; }
		bool GetIsLockAngularX() const { return mbIsLockAngularX; }
		bool GetIsLockAngularY() const { return mbIsLockAngularY; }
		bool GetIsLockAngularZ() const { return mbIsLockAngularZ; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		EBodyType mBodyType;
		Vector3 mLinearVelocity;
		Vector3 mAngularVelocity;

		bool mbIsLockLinearX;
		bool mbIsLockLinearY;
		bool mbIsLockLinearZ;
		bool mbIsLockAngularX;
		bool mbIsLockAngularY;
		bool mbIsLockAngularZ;
	};


}