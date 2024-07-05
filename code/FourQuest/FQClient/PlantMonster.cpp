#define  NOMINMAX
#include "PlantMonster.h"

#include "GameManager.h"
#include "PlantAttack.h"
#include "Attack.h"
#include "HpBar.h"

fq::client::PlantMonster::PlantMonster()
	:mMaxHp(0.f)
	, mHp(0.f)
	, mProjectileSpeed(1.f)
	, mAttackPower(1.f)
	, mAttackPrefab{}
	, mTarget(nullptr)
	, mAnimator(nullptr)
	, mAttackRange(1.f)
	, mGameManager(nullptr)
	, mAttackCoolTime(0.f)
	, mAttackElapsedTime(0.f)
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
	mGameManager =GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	mMaxHp = mHp;
}


void fq::client::PlantMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();
	auto transform = GetComponent<Transform>();

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	attackComponent->SetAttackPower(mAttackPower);

	DirectX::SimpleMath::Vector3 offset = { 0.f,1.f,0.f };
	attackT->SetLocalPosition(transform->GetWorldPosition() + offset);

	// 공격 방향 설정
	auto plantAttack = attackObj->GetComponent<client::PlantAttack>();
	plantAttack->SetMoveSpeed(mProjectileSpeed);

	DirectX::SimpleMath::Vector3 dir = transform->GetWorldMatrix().Forward();
	dir.Normalize();

	plantAttack->SetMoveDirection(dir);

	GetScene()->AddGameObject(attackObj);

	// 공격 쿨타임 관련처리
	mAttackElapsedTime = mAttackCoolTime;

	// TODO : 공격사운드 추가 

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
		mAnimator->SetParameterTrigger("OnHit");
		auto playerAttack = collision.other->GetComponent<client::Attack>();
		float attackPower = playerAttack->GetAttackPower();
		mHp -= attackPower;

		GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

		// 타겟을 자신을 때린 사람으로 바꿉니다 
		SetTarget(playerAttack->GetAttacker());

		// 공격방향의 반대방향으로 몬스터가 바라봅니다
		auto attackDir = playerAttack->GetAttackDirection();
		if (attackDir != client::Attack::NoDirection)
		{

		}

		// PlantMonster 사망 처리 
		if (mHp <= 0.f)
		{
			mAnimator->SetParameterBoolean("IsDead", true);
		}
	}
}

void fq::client::PlantMonster::SetTarget(fq::game_module::GameObject* target)
{
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

	auto directV = targetPos - myPos;
	directV.y = 0.f;
	directV.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0, 1, 0 });
	directionQuaternion.Normalize();

	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(directionQuaternion);

	// UpVector가 뒤집힌 경우
	if (rotationMatrix._22 <= -0.9f)
	{
		rotationMatrix._22 = 1.f;
	}

	transform->SetLocalRotationToMatrix(rotationMatrix);
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

	if (attackAble)
	{
		mAnimator->SetParameterTrigger("OnAttack");
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
