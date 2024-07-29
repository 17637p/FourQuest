#define  NOMINMAX
#include "MonsterSpawner.h"

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

	if (cloneSpawner == nullptr) // 새로 생성해서 복사본을 준다소울라이크
	{
		cloneSpawner = game_module::ObjectPool::GetInstance()->Assign<MonsterSpawner>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSpawner = *this;
	}

	return cloneSpawner;
}

void fq::client::MonsterSpawner::OnUpdate(float dt)
{
	mSpawnElapsedTime = std::max(mSpawnElapsedTime - dt, 0.f);

	// 몬스터 생성
	if (mSpawnElapsedTime == 0.f)
	{
		mSpawnElapsedTime = mSpawnCoolTime;
		mbIsSpawnState = true;
		mAnimator->SetParameterTrigger("Spawn");
	}
}

void fq::client::MonsterSpawner::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mTransform = GetComponent<game_module::Transform>();
	mMonsterGroup = GetComponent<MonsterGroup>();

	mMaxHp = mHp;
	mSpawnElapsedTime = mSpawnCoolTime;
}

void fq::client::MonsterSpawner::Spawn()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMonster);
	auto& monster = *(instance.begin());

	auto spawnerT = GetComponent<game_module::Transform>();
	auto monsterT = monster->GetComponent<game_module::Transform>();

	// 부모 자식 연결
	spawnerT->AddChild(monsterT);

	// 몬스터 스폰위치 설정 
	auto lookDir = DirectX::SimpleMath::Matrix::CreateFromQuaternion(spawnerT->GetWorldRotation()).Right();
	auto monsterPos = lookDir * mSpawnOffset;
	monsterT->SetLocalPosition(monsterPos);

	// 현재 타겟확인
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
}

void fq::client::MonsterSpawner::OnTriggerEnter(const game_module::Collision& collision)
{
	// 피격 처리 
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

			// 스포너 사망 처리 
			if (mHp <= 0.f)
			{
				mAnimator->SetParameterBoolean("IsDead", true);
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
