#define  NOMINMAX
#include "PlantMonster.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "GameManager.h"
#include "LinearAttack.h"
#include "PlantAOEAttack.h"
#include "Attack.h"
#include "HpBar.h"
#include "LevelHepler.h"

fq::client::PlantMonster::PlantMonster()
	:mMaxHp(0.f)
	, mHp(0.f)
	, mProjectileSpeed(1.f)
	, mAttackPower(100.f)
	, mAttackPrefab{}
	, mTarget(nullptr)
	, mAnimator(nullptr)
	, mAttackRange(1.f)
	, mGameManager(nullptr)
	, mAttackCoolTime(0.f)
	, mAttackElapsedTime(0.f)
	, mRotationSpeed(0.1f)
	, mbIsAOEAttacker(false)
{
}

fq::client::PlantMonster::~PlantMonster()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::PlantMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<PlantMonster> cloneMonster = std::dynamic_pointer_cast<PlantMonster>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<PlantMonster>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::PlantMonster::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	
	// 난이도에 따른 공경력 HP 설정
	mAttackPower = mAttackPower * LevelHepler::GetDamageRatio();
	mHp = mHp * LevelHepler::GetHpRatio();
	mMaxHp = mHp;
}


void fq::client::PlantMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();
	auto transform = GetComponent<Transform>();

	DirectX::SimpleMath::Vector3 offset = { 0.f,1.f,0.f };
	attackT->SetLocalPosition(transform->GetWorldPosition() + offset);

	// 공격 방향 설정
	auto plantAttack = attackObj->GetComponent<client::LinearAttack>();
	plantAttack->SetMoveSpeed(mProjectileSpeed);
	DirectX::SimpleMath::Vector3 dir = transform->GetWorldMatrix().Forward();
	dir.Normalize();
	plantAttack->SetMoveDirection(dir);

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	AttackInfo info{};
	info.attacker = attackObj.get();
	info.damage = mAttackPower;
	info.bIsInfinite = false;
	info.remainingAttackCount = 1;
	info.attackDirection = dir;
	attackComponent->Set(info);

	// 공격 쿨타임 관련처리
	mAttackElapsedTime = mAttackCoolTime;

	GetScene()->AddGameObject(attackObj);

	// 원거리 공격사운드  
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MR_Posion", false , fq::sound::EChannel::SE });
}

void fq::client::PlantMonster::EmitAOEAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOEAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();
	auto transform = GetComponent<Transform>();

	DirectX::SimpleMath::Vector3 offset = { 0.f,1.f,0.f };
	attackT->SetLocalPosition(transform->GetWorldPosition() + offset);

	// 포물선 공격 설정
	auto arcAttack = attackObj->GetComponent<PlantAOEAttack>();
	arcAttack->SetmaxRange(mAttackRange);
	arcAttack->SetStartPosition(transform->GetWorldPosition() + offset);
	arcAttack->SetTargetPosition(mTarget->GetComponent<fq::game_module::Transform>()->GetWorldPosition());

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	AttackInfo info{};
	info.attacker = attackObj.get();
	info.damage = mAttackPower;
	info.bIsInfinite = false;
	info.remainingAttackCount = 0b11111111;
	attackComponent->Set(info);

	// 공격 쿨타임 관련처리
	mAttackElapsedTime = mAttackCoolTime;

	GetScene()->AddGameObject(attackObj);
	arcAttack->DetermineArrivalTime();
}

void fq::client::PlantMonster::OnUpdate(float dt)
{
	// Target 삭제 확인
	if (mTarget)
	{
		if (mTarget->IsDestroyed())
			SetTarget(nullptr);
	}

	// 공격 쿨타임 
	mAttackElapsedTime = std::max(mAttackElapsedTime - dt, 0.f);
}

void fq::client::PlantMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// 플레이어 피격 처리 
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<client::Attack>();
		if (playerAttack->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = playerAttack->GetAttackPower();
			mHp -= attackPower;

			GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

			// 타겟을 자신을 때린 사람으로 바꿉니다 
			SetTarget(playerAttack->GetAttacker());

			// 피격 사운드 재생
			playerAttack->PlayHitSound();

			// PlantMonster 사망 처리 
			if (mHp <= 0.f)
			{
				mAnimator->SetParameterBoolean("IsDead", true);
			}

			// 이펙트 방출
			fq::event::OnCreateStateEvent stateEvent;
			stateEvent.gameObject = GetGameObject();
			stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
			if (!stateEvent.RegisterKeyName.empty())
			{
				GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
			}
		}

	}
}

void fq::client::PlantMonster::SetTarget(fq::game_module::GameObject* target)
{
	if (mAnimator == nullptr)
	{
		mAnimator = GetComponent<game_module::Animator>();
		assert(mAnimator);
	}

	// Target이 사라진 경우
	if (target == nullptr)
	{
		mTarget = nullptr;

		mAnimator->SetParameterBoolean("HasTarget", false);
		return;
	}

	mTarget = target->shared_from_this();
	mAnimator->SetParameterBoolean("HasTarget", true);
}

void fq::client::PlantMonster::LookAtTarget()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
		return;

	auto transform = GetComponent<game_module::Transform>();
	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto myPos = transform->GetWorldPosition();
	auto currentRotation = transform->GetWorldRotation();
	auto directV = targetPos - myPos;
	directV.y = 0.f;
	directV.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = currentRotation;

	// UpVector가 뒤집히는 경우에 대한 예외 처리 
	if (directV.z >= 1.f)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation({ 0.f,0.f,1.f }, { 0.f, -1.f, 0.f });
	else if (directV != DirectX::SimpleMath::Vector3::Zero)
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0.f,1.f,0.f });
	directionQuaternion.Normalize();

	DirectX::SimpleMath::Quaternion result =
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, mRotationSpeed);

	transform->SetWorldRotation(result);
}

void fq::client::PlantMonster::CheckTargetInAttackRange()
{
	if (mTarget == nullptr || mTarget->IsDestroyed())
		return;

	auto targetT = mTarget->GetComponent<game_module::Transform>();
	auto plantT = GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto plantpos = plantT->GetWorldPosition();

	float distance = (targetPos - plantpos).Length();;

	bool isInRange = mAttackRange >= distance;

	mAnimator->SetParameterBoolean("InRange", isInRange);

	if (!isInRange)
	{
		SetTarget(nullptr);
	}
}

void fq::client::PlantMonster::CheckAttackAble()
{
	bool attackAble = mAttackElapsedTime == 0.f;

	if (attackAble && !mbIsAOEAttacker)
	{
		mAnimator->SetParameterTrigger("OnAttack");
	}
	else if (attackAble && mbIsAOEAttacker)
	{
		mAnimator->SetParameterTrigger("OnAOEAttack");
	}
}

void fq::client::PlantMonster::FindTarget()
{
	game_module::GameObject* target = nullptr;

	auto monsterT = GetComponent<game_module::Transform>();
	auto monsterPos = monsterT->GetWorldPosition();

	float minDistance = mAttackRange;

	// 가장 가까운 플레이어를 쿼리
	for (const auto& player : mGameManager->GetPlayers())
	{
		if (player->GetTag() == game_module::ETag::Soul)
		{
			continue;
		}

		auto playerT = player->GetComponent<game_module::Transform>();
		auto playerPos = playerT->GetWorldPosition();

		float distance = (playerPos - monsterPos).Length();


		if (distance <= minDistance)
		{
			target = player.get();
			minDistance = distance;
		}
	}

	if (target)
	{
		SetTarget(target);
	}
}
