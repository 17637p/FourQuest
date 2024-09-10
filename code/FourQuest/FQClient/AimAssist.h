#pragma once

#include <list>

#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class GameObject;
}

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

		void SetNearMonsterDirection();

	private:
		void OnTriggerEnter(const game_module::Collision& collision) override;
		void OnTriggerExit(const game_module::Collision& collision) override;

		void OnUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		//std::unordered_map<unsigned int, std::shared_ptr<game_module::GameObject>> mQueryObject;
		float mTheta;
		std::list<game_module::GameObject*> mInRangeMonsters; // 범위 안에 있는 몬스터

		friend void RegisterMetaData();
	};
}