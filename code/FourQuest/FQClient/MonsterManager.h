#pragma once


#include "../FQGameModule/Component.h"
#include "../FQGameModule/EventManager.h"

namespace fq::client
{
	class MonsterManager : public fq::game_module::Component
	{
		using EventHandler = game_module::EventHandler;
		using MonsterContainer = std::vector<std::shared_ptr<game_module::GameObject>>;

	public:
		MonsterManager();
		~MonsterManager();
		MonsterManager(const MonsterManager& other);
		MonsterManager& operator=(const MonsterManager& other);

		/// <summary>
		/// ���� �� ���� ���ڸ� ��ȯ�մϴ�.
		/// </summary>
		int GetMonsterSize()const { return static_cast<int>(mMonsters.size()); }


		/// <summary>
		/// ���͸� ���� �����̳ʸ� ��ȯ�մϴ�.
		/// </summary>
		const MonsterContainer& GetMonsters()const { return mMonsters; }

		/// <summary>
		/// ������ �����뵵�� �Լ��Դϴ�. ���X
		/// </summary>
		void SetMonsterSize(int size) {};

	private:
		void OnAwake() override;
		void OnDestroy() override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		MonsterContainer mMonsters;

		EventHandler mOnDestroyHandler;
		EventHandler mAddGameObjectHandler;
	};
}