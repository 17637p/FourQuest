#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	//몬스터들을 그룹화하고 같은 타겟을 설정하는 역할을 합니다
	class MonsterGroup :public game_module::Component
	{
	public:
		/// <summary>
		/// 몬스터객체에 그룹이 존재하면 그룹을 반환하고
		/// 그렇지 않으면 nullptr을 반환합니다
		/// </summary>
		static MonsterGroup* GetMonsterGroup(fq::game_module::GameObject* monster);

	public:
		MonsterGroup();
		~MonsterGroup();

		/// <summary>
		/// 몬스터 그룹에 몬스터를 등록합니다 
		/// </summary>
		void Register(fq::game_module::GameObject* monster);

		/// <summary>
		/// 몬스터들에게 타겟을 발견했다고 알립니다 
		/// </summary>
		void AnnounceFindedTarget(fq::game_module::GameObject* target);

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