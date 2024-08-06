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
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 선속도를 반환합니다 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetLinearVelocity() const { return mLinearVelocity; }

		/// <summary>
		/// 선속도를 더합니다.
		/// </summary>
		void AddLinearVelocity(Vector3 velocity);

		/// <summary>
		/// 선속도를 설정합니다 
		/// </summary>
		void SetLinearVelocity(fq::game_module::RigidBody::Vector3 val) { mLinearVelocity = val; }
		
		/// <summary>
		/// 각속도를 반환합니다 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetAngularVelocity() const { return mAngularVelocity; }
		
		/// <summary>
		/// 각속도를 설정합니다 
		/// </summary>
		void SetAngularVelocity(fq::game_module::RigidBody::Vector3 val) { mAngularVelocity = val; }

		/// <summary>
		/// 리지드 바디 타입을 반환합니다
		/// </summary>
		fq::game_module::RigidBody::EBodyType GetBodyType() const { return mBodyType; }
		
		/// <summary>
		/// 리지드 바디 타입을 설정합니다
		/// </summary>
		void SetBodyType(fq::game_module::RigidBody::EBodyType val) { mBodyType = val; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		EBodyType mBodyType;
		Vector3 mLinearVelocity;
		Vector3 mAngularVelocity;
	};


}