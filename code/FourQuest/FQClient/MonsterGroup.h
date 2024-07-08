#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	//몬스터들을 그룹화하고 같은 타겟을 설정하는 역할을 합니다
	class MonsterGroup :public game_module::Component
	{
	public:
		MonsterGroup();
		~MonsterGroup();

		/// <summary>
		/// 몬스터 그룹에 몬스터를 등록합니다 
		/// </summary>
		void Register(fq::game_module::GameObject* monster);

		/// <summary>
		/// 타겟을 설정합니다
		/// </summary>
		void SetTarget(fq::game_module::GameObject* target);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;
	private:
		std::vector<std::shared_ptr<game_module::GameObject>> mMonsters;
		std::shared_ptr<fq::game_module::GameObject> mTarget;
	};
}