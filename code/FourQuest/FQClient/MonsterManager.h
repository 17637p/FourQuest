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
		/// 현재 총 몬스터 숫자를 반환합니다.
		/// </summary>
		int GetMonsterSize()const { return static_cast<int>(mMonsters.size()); }


		/// <summary>
		/// 몬스터를 담은 컨테이너를 반환합니다.
		/// </summary>
		const MonsterContainer& GetMonsters()const { return mMonsters; }

		/// <summary>
		/// 에디터 디버깅용도의 함수입니다. 사용X
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