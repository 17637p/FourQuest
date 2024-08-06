#pragma once


#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// 에임 보정 
	/// </summary>
	class AimAssist : public game_module::Component
	{
	public:
		AimAssist();
		~AimAssist();

	private:
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		std::unordered_map<unsigned int, std::shared_ptr<game_module::GameObject>> mQueryObject;
	};


}