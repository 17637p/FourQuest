#pragma once
#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	//���͵��� �׷�ȭ�ϰ� ���� Ÿ���� �����ϴ� ������ �մϴ�
	class MonsterGroup :public game_module::Component
	{
	public:
		/// <summary>
		/// ���Ͱ�ü�� �׷��� �����ϸ� �׷��� ��ȯ�ϰ�
		/// �׷��� ������ nullptr�� ��ȯ�մϴ�
		/// </summary>
		static MonsterGroup* GetMonsterGroup(fq::game_module::GameObject* monster);

	public:
		MonsterGroup();
		~MonsterGroup();

		/// <summary>
		/// ���� �׷쿡 ���͸� ����մϴ� 
		/// </summary>
		void Register(fq::game_module::GameObject* monster);

		/// <summary>
		/// ���͵鿡�� Ÿ���� �߰��ߴٰ� �˸��ϴ� 
		/// </summary>
		void AnnounceFindedTarget(fq::game_module::GameObject* target);

		/// <summary>
		/// ���͵��� ��ȯ�մϴ� 
		/// </summary>
		const std::vector<std::shared_ptr<game_module::GameObject>>& GetMonsters()const { return mMonsters; }

		/// <summary>
		/// ���ͱ׷��� ���� ������ ������ ��ȯ�մϴ� 
		/// </summary>
		int GetRemainMonsterSize()const;

		/// <summary>
		/// ���� �׷쿡 ��ϵ� ��� ������ ������ ��ȯ�մϴ�
		/// </summary>
		int GetAllMonsterSize()const;

		/// <summary>
		/// Ÿ���� ��ȯ�մϴ�
		/// </summary>
		fq::game_module::GameObject* GetTarget()const;

		/// <summary>
		/// Ÿ���� �����մϴ�
		/// </summary>
		void SetTarget(fq::game_module::GameObject* target);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnAwake() override;
		void OnUpdate(float dt) override;
	private:
		std::vector<std::shared_ptr<game_module::GameObject>> mMonsters;
		std::shared_ptr<fq::game_module::GameObject> mTarget;

		int mGroupIndex;
		int mMonsterCount;

		friend void RegisterMetaData();
	};
}