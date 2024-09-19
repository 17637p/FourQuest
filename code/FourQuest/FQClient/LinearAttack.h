#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// 원거리 몬스터가 발사하는 투사체 설정 
	/// </summary>
	class LinearAttack :public game_module::Component 
 	{
	public:
		LinearAttack();
		~LinearAttack();

		DirectX::SimpleMath::Vector3 GetMoveDirection() const { return mMoveDirection; }
		void SetMoveDirection(DirectX::SimpleMath::Vector3 val) { mMoveDirection = val; }
		float GetMoveSpeed() const { return mMoveSpeed; }
		void SetMoveSpeed(float val) { mMoveSpeed = val; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnUpdate(float dt) override;

	private:
		DirectX::SimpleMath::Vector3 mMoveDirection;
		float mMoveSpeed;
	};
}