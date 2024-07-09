#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	//���͵��� �׷�ȭ�ϰ� ���� Ÿ���� �����ϴ� ������ �մϴ�
	class MonsterGroup :public game_module::Component
	{
	public:
		MonsterGroup();
		~MonsterGroup();

		/// <summary>
		/// ���� �׷쿡 ���͸� ����մϴ� 
		/// </summary>
		void Register(fq::game_module::GameObject* monster);

		/// <summary>
		/// Ÿ���� �����մϴ�
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