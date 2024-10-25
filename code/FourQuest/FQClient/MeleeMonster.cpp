#define NOMINMAX
#include "MeleeMonster.h"

#include <random>
#include <algorithm>

#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/Socket.h"
#include "../FQGameModule/BoxCollider.h"

#include "Attack.h"
#include "ArrowAttack.h"
#include "GameManager.h"
#include "HpBar.h"
#include "MonsterGroup.h"
#include "KnockBack.h"
#include "Player.h"
#include "LevelHepler.h"
#include "PlayerInfoVariable.h"
#include "PlayerDummy.h"
#include "ClientHelper.h"
#include "MonsterHP.h"

fq::client::MeleeMonster::MeleeMonster()
	:mMaxHp(0.f)
	, mHp(100.f)
	, mAttackPower(10.f)
	, mMoveSpeed(1.f)
	, mAcceleration(1.f)
	, mAttackRange(10.f)
	, mPatrolRange(1.f)
	, mDetectRange(3.f)
	, mAttackCoolTime(3.f)
	, mAttackElapsedTime(0.f)
	, mGameManager(nullptr)
	, mMonsterHpUI(nullptr)
	, mAnimator(nullptr)
	, mTarget(nullptr)
	, mKnockBack(nullptr)
	, mStartPosition{}
	, mPatrolDestination{}
	, mTransform(nullptr)
	, mAttackOffset(1.f)
	, mDummyTraceDurationTime(0.f)
	, mbUseDummyTraceRandomRange(false)
	, mDummyDurationRandomRangeMin(0.f)
	, mDummyDurationRandomRangeMax(0.f)
	, mCurrentDummyTraceDurationTime(0.f)
	, mDummyTraceElapsedTime(0.f)
	, mIsDummyTarget(false)
	, mArrowImotalTime(0.1f)
	, mArrowHitDuration()
{}

fq::client::MeleeMonster::~MeleeMonster()
{}

std::shared_ptr<fq::game_module::Component> fq::client::MeleeMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MeleeMonster> cloneMonster = std::dynamic_pointer_cast<MeleeMonster>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MeleeMonster>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MeleeMonster::SetTarget(game_module::GameObject* target)
{
	if (mAnimator == nullptr)
	{
		mAnimator = GetComponent<game_module::Animator>();
		assert(mAnimator);
	}

	if (target == nullptr)
	{
		mTarget = nullptr;
		mAnimator->SetParameterBoolean("HasTarget", false);
		mIsDummyTarget = false;
		return;
	}

	// 더미 타겟인지 체크
	mIsDummyTarget = target->GetComponent<PlayerDummy>() != nullptr;
	if (mIsDummyTarget)
	{
		mDummyTraceElapsedTime = 0.f;
		float random = helper::RandomGenerator::GetInstance().GetRandomNumber(mDummyDurationRandomRangeMin, mDummyDurationRandomRangeMax);
		mCurrentDummyTraceDurationTime = mDummyTraceDurationTime + random;
	}

	mTarget = target->shared_from_this();
	mAnimator->SetParameterBoolean("HasTarget", true);
}

void fq::client::MeleeMonster::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mStartPosition = mTransform->GetWorldPosition();
	mAnimator = GetComponent<game_module::Animator>();
	mKnockBack = GetComponent<KnockBack>();

	for (auto child : mTransform->GetChildren())
	{
		auto hpBar = child->GetComponent<MonsterHP>();
		if (hpBar)
		{
			mMonsterHpUI = hpBar;
		}
	}

	if (mMonsterHpUI == nullptr)
	{
		spdlog::warn("name:{} meleee monster has not monster hp ui", GetGameObject()->GetName());
	}

	// 난이도에 따른 공격력 HP 설정
	mAttackPower = mAttackPower * LevelHepler::GetDamageRatio();
	mHp = mHp * LevelHepler::GetHpRatio();
	mMaxHp = mHp;

	// Agent 설정
	auto agent = GetComponent<game_module::NavigationAgent>();
	agent->SetSpeed(mMoveSpeed);
	agent->SetAcceleration(mAcceleration);
	agent->SetRadius(0.3f);

	// GameManager 연결
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	if (mTarget)
	{
		SetTarget(mTarget.get());
	}
}

void fq::client::MeleeMonster::EmitAttack()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackPrefab);
	auto& attackObj = *(instance.begin());

	auto attackT = attackObj->GetComponent<Transform>();

	// 공격 트랜스폼 설정
	auto attackPos = mTransform->GetWorldPosition();
	auto scale = attackT->GetWorldScale();
	auto rotation = mTransform->GetWorldRotation();

	auto rotationMat = DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation);
	auto foward = rotationMat.Forward();
	attackPos += foward * mAttackOffset;
	attackT->GenerateWorld(attackPos, rotation, scale);

	// 공격 정보 설정
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackInfo.attacker = GetGameObject();
	attackInfo.damage = mAttackPower;
	attackInfo.attackDirection = foward;
	attackInfo.hitSound = "MM_Attack_hit";
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);

	// 공격 쿨타임 관련처리
	mAttackElapsedTime = mAttackCoolTime;

	// 공격 사운드
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Attack", false , fq::sound::EChannel::SE });
}


std::shared_ptr<fq::game_module::GameObject> fq::client::MeleeMonster::EmitAttackEffect()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttackEffect);
	auto& effentObj = *(instance.begin());

	auto effectT = effentObj->GetComponent<game_module::Transform>();
	effectT->SetParent(mTransform);

	GetScene()->AddGameObject(effentObj);

	return effentObj;
}

void fq::client::MeleeMonster::Move(DirectX::SimpleMath::Vector3 destination)
{
	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();

	if (agent)
		agent->MoveTo(destination);
}

void fq::client::MeleeMonster::Patrol()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	// 순찰 위치 정하기 
	std::uniform_real_distribution<float> patrolDis(0, mPatrolRange);
	float distance = patrolDis(gen);

	std::uniform_real_distribution<float> radianDis(0, DirectX::XM_2PI);
	float radian = radianDis(gen);

	mPatrolDestination = mStartPosition;
	mPatrolDestination.x += std::cos(radian) * distance;
	mPatrolDestination.z += std::sin(radian) * distance;

	fq::game_module::NavigationAgent* agent = GetComponent<fq::game_module::NavigationAgent>();

	Move(mPatrolDestination);
}

void fq::client::MeleeMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// 플레이어 공격 피격 처리
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = playerAttack->GetAttackPower();

			// 타겟은 자신을 때린 사람으로 바꿉니다 
			SetTarget(playerAttack->GetAttacker());

			// 넉백처리 
			if (playerAttack->HasKnockBack())
			{
				auto type = playerAttack->GetKnockBackType();
				float power = playerAttack->GetKnockBackPower();

				if (type == EKnockBackType::Fixed)
				{
					DirectX::SimpleMath::Vector3 direction = playerAttack->GetAttackDirection();
					mKnockBack->AddKnockBack(power, direction);
				}
				else if (type == EKnockBackType::TargetPosition)
				{
					auto monsterPos = mTransform->GetWorldPosition();
					monsterPos.y = 0.f;
					auto attackPos = playerAttack->GetAttackPosition();
					attackPos.y = 0.f;

					auto knockBackDir = monsterPos - attackPos;
					knockBackDir.Normalize();

					mKnockBack->AddKnockBack(power, knockBackDir);
				}
				else if (type == EKnockBackType::TargetPositionAndDirectionByAngle)
				{
					// 현재는 콜라이더가 항상 플레이어 앞쪽에 위치함을 가정함
					auto monsterPos = mTransform->GetWorldPosition();
					monsterPos.y = 0.f;
					auto attackPos = playerAttack->GetAttackPosition();
					attackPos.y = 0.f;

					auto pushedDir = monsterPos - attackPos;
					pushedDir.Normalize();

					// 플레이어를 원점으로 몬스터와 이루는 각도가 작을수록 방향의 영향을 많이 받음
					const auto& invDirection = -playerAttack->GetAttackDirection();
					float directionRatio = invDirection.Dot(pushedDir) * 0.5 + 0.5;
					directionRatio = std::clamp<float>(directionRatio, 0.f, 1.f);

					auto knockBackDir = pushedDir * (1 - directionRatio) * playerAttack->GetTargetPosRatio() + playerAttack->GetAttackDirection() * directionRatio * playerAttack->GetDirectionRatio();
					//knockBackDir.Normalize();

					mKnockBack->AddKnockBack(power, knockBackDir);
				}
				else if (type == EKnockBackType::TargetPositionAndKnockDown)
				{
					auto monsterPos = mTransform->GetWorldPosition();
					monsterPos.y = 0.f;
					auto attackPos = playerAttack->GetAttackPosition();
					attackPos.y = 0.f;

					auto knockBackDir = monsterPos - attackPos;
					knockBackDir.Normalize();

					mKnockBack->AddKnockBack(power, knockBackDir);

					// 몬스터를 넘어트리는 상태 변화 호출
					// mAnimator->SetParameterBoolean("IsKockDown", true);
				}
			}

			// HP 설정
			mHp -= attackPower;
			if (mMonsterHpUI)
				mMonsterHpUI->DecreaseHp(attackPower / mMaxHp);

			// 피격 사운드 재생
			playerAttack->PlayHitSound();

			// 이펙트 방출
			fq::event::OnCreateStateEvent stateEvent;
			stateEvent.gameObject = GetGameObject();
			stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
			if (!stateEvent.RegisterKeyName.empty())
			{
				GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
			}

			// 사망처리 
			if (mHp <= 0.f)
			{
				if (playerAttack->GetAttacker() != nullptr)
				{
					auto attackerID = playerAttack->GetAttacker()->GetComponent<Player>()->GetPlayerID();
					if (attackerID == 0)
					{
						PlayerInfoVariable::Player1Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player1Monster);
					}
					if (attackerID == 1)
					{
						PlayerInfoVariable::Player2Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player2Monster);
					}
					if (attackerID == 2)
					{
						PlayerInfoVariable::Player3Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player3Monster);
					}
					if (attackerID == 3)
					{
						PlayerInfoVariable::Player4Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player4Monster);
					}
				}
				mAnimator->SetParameterBoolean("IsDead", true);
				destroySocketCollider();
			}
		}
	}
}

void fq::client::MeleeMonster::ChaseTarget()
{
	if (mTarget == nullptr)
	{
		SetTarget(nullptr);
		return;
	}
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		auto dummyOrNull = playerOrNull->CreateDummyOrNull();

		if (dummyOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		SetTarget(dummyOrNull);
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();
	auto targetPos = targetT->GetWorldPosition();
	Move(targetPos);
}

void fq::client::MeleeMonster::DetectTarget()
{
	auto monsterPos = mTransform->GetWorldPosition();

	for (const auto& player : mGameManager->GetPlayers())
	{
		if (!player->HasComponent<Player>())
			continue;

		auto playerT = player->GetComponent<game_module::Transform>();
		auto playerPos = playerT->GetWorldPosition();
		float distance = (monsterPos - playerPos).Length();

		if (distance <= mDetectRange)
		{
			SetTarget(player.get());
			mAnimator->SetParameterBoolean("FindTarget", true);
		}
	}

	GetScene()->ViewComponents<PlayerDummy>(
		[this, monsterPos](fq::game_module::GameObject& object, PlayerDummy& camera)
		{
			auto dummyPlayerT = object.GetComponent<game_module::Transform>();
			auto playerPos = dummyPlayerT->GetWorldPosition();
			float distance = (monsterPos - playerPos).Length();

			if (distance <= mDetectRange)
			{
				SetTarget(&object);
				mAnimator->SetParameterBoolean("FindTarget", true);
			}
		}
	);
}

void fq::client::MeleeMonster::CheckTargetInAttackRange()
{
	if (mTarget == nullptr)
	{
		SetTarget(nullptr);
		return;
	}
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		auto dummyOrNull = playerOrNull->CreateDummyOrNull();

		if (dummyOrNull == nullptr)
		{
			SetTarget(nullptr);
			return;
		}

		SetTarget(dummyOrNull);
	}

	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto monsterPos = mTransform->GetWorldPosition();

	float distance = (targetPos - monsterPos).Length();
	bool isInAttackRange = mAttackRange >= distance;

	mAnimator->SetParameterBoolean("InAttackRange", isInAttackRange);
}

void fq::client::MeleeMonster::CheckAttackAble() const
{
	bool attackAble = mAttackElapsedTime == 0.f;

	if (attackAble)
	{
		mAnimator->SetParameterTrigger("OnAttack");
	}
}

void fq::client::MeleeMonster::OnUpdate(float dt)
{
	mAttackElapsedTime = std::max(0.f, mAttackElapsedTime - dt);
	mArrowHitDuration += dt;

	// 현재 타겟이 더미 타겟인지 체크
	if (mIsDummyTarget)
	{
		mDummyTraceElapsedTime += dt;

		if (mCurrentDummyTraceDurationTime < mDummyTraceElapsedTime)
		{
			SetTarget(nullptr);
		}
	}
}

void fq::client::MeleeMonster::AnnounceFindedTarget()
{
	auto group = MonsterGroup::GetMonsterGroup(GetGameObject());

	if (group)
	{
		group->AnnounceFindedTarget(mTarget.get());
	}
}

void fq::client::MeleeMonster::LookAtTarget()
{
	if (mTarget == nullptr)
	{
		return;
	}
	if (mTarget->IsDestroyed())
	{
		auto playerOrNull = mTarget->GetComponent<Player>();

		if (playerOrNull == nullptr)
		{
			return;
		}

		auto dummyOrNull = playerOrNull->CreateDummyOrNull();

		if (dummyOrNull == nullptr)
		{
			return;
		}

		SetTarget(dummyOrNull);
	}

	constexpr float RotationSpeed = 0.1f;

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
		DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, RotationSpeed);

	mTransform->SetWorldRotation(result);
}

void fq::client::MeleeMonster::DestroyMonsterHPUI()
{
	if (mMonsterHpUI)
	{
		GetScene()->DestroyGameObject(mMonsterHpUI->GetGameObject());
		mMonsterHpUI = nullptr;
	}
	else
	{
		spdlog::warn("MeleeMonster has not monster ui");
	}

}

void fq::client::MeleeMonster::HitArrow(fq::game_module::GameObject* object)
{
	if (mArrowHitDuration < mArrowImotalTime)
		return;

	// 플레이어 공격 피격 처리
	if (object->GetTag() == game_module::ETag::FinalArrow)
	{
		mArrowHitDuration = 0.f;
		auto playerAttack = object->GetComponent<Attack>();

		if (playerAttack->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = playerAttack->GetAttackPower();

			// 타겟은 자신을 때린 사람으로 바꿉니다 
			SetTarget(playerAttack->GetAttacker());

			// HP 설정
			mHp -= attackPower;
			if (mMonsterHpUI)
				mMonsterHpUI->DecreaseHp(attackPower / mMaxHp);

			// 피격 사운드 재생
			playerAttack->PlayHitSound();

			// 이펙트 방출
			fq::event::OnCreateStateEvent stateEvent;
			stateEvent.gameObject = GetGameObject();
			stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
			if (!stateEvent.RegisterKeyName.empty())
			{
				GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
			}

			// 사망처리 
			if (mHp <= 0.f)
			{
				if (playerAttack->GetAttacker() != nullptr)
				{
					auto attackerID = playerAttack->GetAttacker()->GetComponent<Player>()->GetPlayerID();
					if (attackerID == 0)
					{
						PlayerInfoVariable::Player1Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player1Monster);
					}
					if (attackerID == 1)
					{
						PlayerInfoVariable::Player2Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player2Monster);
					}
					if (attackerID == 2)
					{
						PlayerInfoVariable::Player3Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player3Monster);
					}
					if (attackerID == 3)
					{
						PlayerInfoVariable::Player4Monster += 1;
						spdlog::trace("Player1Monster: {}", PlayerInfoVariable::Player4Monster);
					}
				}
				mAnimator->SetParameterBoolean("IsDead", true);
				destroySocketCollider();
			}
		}
	}
}

void fq::client::MeleeMonster::destroySocketCollider()
{
	// 소켓을 가지고 있는 자식 오브젝트의 박스 콜라이더 삭제
	for (auto childObject : GetGameObject()->GetChildren())
	{
		auto socket = childObject->GetComponent<fq::game_module::Socket>();

		if (socket)
			childObject->RemoveComponent<fq::game_module::BoxCollider>();
	}
}