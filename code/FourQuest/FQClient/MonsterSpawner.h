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
	/// ���� ������
	/// </summary>
	class MonsterSpawner : public fq::game_module::Component
	{
	public:
		MonsterSpawner();
		~MonsterSpawner();

		/// <summary>
		/// ���͸� �����մϴ� 
		/// </summary>
		void Spawn();

		/// <summary>
		/// ���� �����ʴ� �ı��մϴ�
		/// </summary>
		void Destroy();

		/// <summary>
		/// ���� �����ʰ� ȭ�쿡 �½��ϴ�
		/// </summary>
		void HitArrow(fq::game_module::GameObject* object);

		/// �÷��̾ �����Ͽ� ���ͽ����ʸ� Ȱ��ȭ�մϴ� 
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