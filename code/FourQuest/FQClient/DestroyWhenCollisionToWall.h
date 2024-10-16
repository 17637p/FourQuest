#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// 벽에 부딪히면 삭제하는 컴포넌트
	/// </summary>
	class DestroyWhenCollisionToWall : public fq::game_module::Component
	{
	public:
		DestroyWhenCollisionToWall();
		~DestroyWhenCollisionToWall();

	private:
		virtual void OnTriggerEnter(const fq::game_module::Collision& collision) override;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;
		entt::meta_handle GetHandle() override;
		
	};
}