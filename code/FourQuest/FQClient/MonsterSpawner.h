#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::game_module
{
	class Transform;
	class Animator;
}

namespace fq::client
{
	class MonsterGroup;

	/// <summary>
	/// 몬스터 스포너
	/// </summary>
	class MonsterSpawner : public fq::game_module::Component
	{
	public:
		MonsterSpawner();
		~MonsterSpawner();

		/// <summary>
		/// 몬스터를 스폰합니다 
		/// </summary>
		void Spawn();

		/// <summary>
		/// 몬스터 스포너는 파괴합니다
		/// </summary>
		void Destroy();

		/// <summary>
		/// 몬스터 스포너가 화살에 맞습니다
		/// </summary>
		void HitArrow(fq::game_module::GameObject* object);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt) override;
		void OnStart() override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
	private:
		fq::game_module::Animator* mAnimator;
		fq::game_module::Transform* mTransform;
		MonsterGroup* mMonsterGroup;

		bool mbIsSpawnState;
		float mSpawnCoolTime;
		float mSpawnElapsedTime;
		float mSpawnOffset;

		float mHp;
		float mMaxHp;

		fq::game_module::PrefabResource mMonster;

		friend void RegisterMetaData();
	};
}