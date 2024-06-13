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
		/// true�� static body, false �� dynamic body �Դϴ�
		/// </summary>
		bool IsStatic() const { return mbIsStatic; }

		/// <summary>
		/// true�� static body, false �� dynamic body �Դϴ�
		/// </summary>
		void SetStatic(bool isStatic) { mbIsStatic = isStatic; }

		/// <summary>
		/// ���ӵ��� ��ȯ�մϴ� 
		/// </summary>
		fq::game_module::RigidBody::Vector3 GetLinearVelocity() const { return mLinearVelocity; }
		
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

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mbIsStatic;
		Vector3 mLinearVelocity;
		Vector3 mAngularVelocity;
	};


}