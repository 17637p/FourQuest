#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// °©¿ÊÆÄ±« ÄÄÆ÷³ÍÆ® 
	/// </summary>
	class ArmourDestroyer : public fq::game_module::Component
	{
	public:
		void OnTriggerEnter(const game_module::Collision& collision) override;

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */) const override;
		entt::meta_handle GetHandle() override { return *this; }

	private:
		bool mbIsRemainDeadAromour = true;

		friend void RegisterMetaData();
	};
}