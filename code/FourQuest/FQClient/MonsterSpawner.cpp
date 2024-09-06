#define  NOMINMAX
#include "MonsterSpawner.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/NavigationAgent.h"
#include "Attack.h"
#include "MonsterGroup.h"
#include "MeleeMonster.h"
#include "HpBar.h"

fq::client::MonsterSpawner::MonsterSpawner()
	:mSpawnCoolTime(10.f)
	, mSpawnElapsedTime(0.f)
	, mHp(1000.f)
	, mMaxHp(1000.f)
	, mSpawnOffset(2.f)
	, mbIsSpawnState(false)
	, mTransform(nullptr)
	, mMonsterGroup(nullptr)
	, mAnimator(nullptr)
{}

fq::client::MonsterSpawner::~MonsterSpawner()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MonsterSpawner::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpawner = std::dynamic_pointer_cast<MonsterSpawner>(clone);

	if (cloneSpawner == nullptr) // ���� �����ؼ� ���纻�� �شټҿ����ũ
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawner>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSpawner = *this;
	}

	return cloneSpawner;
}

void fq::client::MonsterSpawner::OnUpdate(float dt)
{
	mSpawnElapsedTime = std::max(mSpawnElapsedTime - dt, 0.f);

	// ���� ����
	if (mSpawnElapsedTime == 0.f)
	{
		mSpawnElapsedTime = mSpawnCoolTime;
		mbIsSpawnState = true;
		mAnimator->SetParameterTrigger("Spawn");
	}

	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetAgentState();
}

void fq::client::MonsterSpawner::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mTransform = GetComponent<game_module::Transform>();
	mMonsterGroup = GetComponent<MonsterGroup>();

	mMaxHp = mHp;
	mSpawnElapsedTime = mSpawnCoolTime;

	auto agent = GetComponent<game_module::NavigationAgent>();
	if (agent)
	{
		agent->SetSpeed(0.f);
		agent->SetAcceleration(0.f);
		agent->SetRadius(3.f);
		agent->SetSyncRotationWithMovementDirection(false);
		//agent->SetAgentState();
	}

}

void fq::client::MonsterSpawner::Spawn()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMonster);
	auto& monster = *(instance.begin());

	auto spawnerT = GetComponent<game_module::Transform>();
	auto monsterT = monster->GetComponent<game_module::Transform>();

	// �θ� �ڽ� ����
	spawnerT->AddChild(monsterT);

	// ���� ������ġ ���� 
	auto lookDir = DirectX::SimpleMath::Matrix::CreateFromQuaternion(spawnerT->GetWorldRotation()).Right();
	auto monsterPos = lookDir * mSpawnOffset;
	monsterT->SetLocalPosition(monsterPos);

	// ���� Ÿ��Ȯ��
	auto target = mMonsterGroup->GetTarget();
	if (target)
	{
		// Melee
		if (monster->HasComponent<MeleeMonster>())
		{
			monster->GetComponent<MeleeMonster>()->SetTarget(target);
		}
	}

	mMonsterGroup->Register(monster.get());

	GetScene()->AddGameObject(monster);
	mbIsSpawnState = false;

	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "Spawner_Spon", false ,  fq::sound::EChannel::SE });
}

void fq::client::MonsterSpawner::OnTriggerEnter(const game_module::Collision& collision)
{
	// �ǰ� ó�� 
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			float damage = playerAttack->GetAttackPower();
			mHp -= damage;
			GetComponent<HpBar>()->DecreaseHp(damage / mMaxHp);

			if (!mbIsSpawnState)
			{
				mAnimator->SetParameterTrigger("OnHit");
			}

			// �ǰ� ���� ���
			playerAttack->PlayHitSound();

			// ������ ��� ó�� 
			if (mHp <= 0.f)
			{
				mAnimator->SetParameterBoolean("IsDead", true);
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "Spawner_Death", false , fq::sound::EChannel::SE });
			}
		}
	}
}

void fq::client::MonsterSpawner::Destroy()
{
	for (const auto& monster : mMonsterGroup->GetMonsters())
	{
		auto transform = monster->GetComponent<game_module::Transform>();

		auto matrix = transform->GetWorldMatrix();
		transform->SetParent(nullptr);
		transform->SetWorldMatrix(matrix);
	}

	GetScene()->DestroyGameObject(GetGameObject());
}
