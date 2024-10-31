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
	class MonsterHP;

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

		/// 플레이어를 감지하여 몬스터스포너를 활성화합니다 
		/// </summary>
		void DetectPlayer(game_module::GameObject* target);

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnUpdate(float dt) override;
		void OnStart() override;
		void OnTriggerEnter(const game_module::Collision& collision) override;
	
		void onHitRimLight();

	private:
		fq::game_module::Animator* mAnimator;
		fq::game_module::Transform* mTransform;
		MonsterGroup* mMonsterGroup;
		MonsterHP* mMonsterHp;

		bool mbIsSpawnState;
		float mSpawnCoolTime;
		float mSpawnElapsedTime;
		float mSpawnOffset;

		bool mbOnHitRimLight;
		float mHitElapsedTime;

		float mHp;
		float mMaxHp;

		fq::game_module::PrefabResource mMonster;

		friend void RegisterMetaData();
	};
}