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
		/// true면 static body, false 는 dynamic body 입니다
		/// </summary>
		bool IsStatic() const { return mbIsStatic; }

		/// <summary>
		/// true면 static body, false 는 dynamic body 입니다
		/// </summary>
		void SetStatic(bool isStatic) { mbIsStatic = isStatic; }

		/// <summary>
		/// 선속도를 반환합니다 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetLinearVelocity() const { return mLinearVelocity; }
		
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

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mbIsStatic;
		Vector3 mLinearVelocity;
		Vector3 mAngularVelocity;
	};


}