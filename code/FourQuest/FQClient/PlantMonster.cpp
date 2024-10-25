#define  NOMINMAX
#include "PlantMonster.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/BoxCollider.h"
#include "../FQGameModule/Socket.h"

#include "GameManager.h"
#include "LinearAttack.h"
#include "PlantAOEAttack.h"
#include "Attack.h"
#include "HpBar.h"
#include "LevelHepler.h"
#include "Player.h"
#include "PlayerInfoVariable.h"
#include "PlayerDummy.h"
#include "MonsterHP.h"
#include "ClientHelper.h"

#include <spdlog/spdlog.h>

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
	, mMonsterHpUI(nullptr)
{
}

fq::client::PlantMonster::~PlantMonster()
{

}


std::shared_ptr<fq::game_module::Component> fq::client::PlantMonster::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<PlantMonster> cloneMonster = std::dynamic_pointer_cast<PlantMonster>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<PlantMonster>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::PlantMonster::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mGameManager = GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>();

	// ���̵��� ���� ����� HP ����
	mAttackPower = mAttackPower * LevelHepler::GetDamageRatio();
	mHp = mHp * LevelHepler::GetHpRatio();
	mMaxHp = mHp;

	for (auto child : GetTransform()->GetChildren())
	{
		auto hpBar = child->GetComponent<MonsterHP>();
		if (hpBar)
		{
			mMonsterHpUI = hpBar;
		}
	}

	if (mMonsterHpUI == nullptr)
	{
		spdlog::warn("name:{} plant monster has not MonsterHPUI", GetGameObject()->GetName());
	}
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

	// ���� ���� ����
	auto plantAttack = attackObj->GetComponent<client::LinearAttack>();
	plantAttack->SetMoveSpeed(mProjectileSpeed);
	DirectX::SimpleMath::Vector3 dir = transform->GetWorldMatrix().Forward();
	dir.Normalize();
	plantAttack->SetMoveDirection(dir);

	// ���� ����
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	AttackInfo info{};
	info.attacker = attackObj.get();
	info.damage = mAttackPower;
	info.bIsInfinite = false;
	info.remainingAttackCount = 1;
	info.attackDirection = dir;
	attackComponent->Set(info);

	// ���� ��Ÿ�� ����ó��
	mAttackElapsedTime = mAttackCoolTime;

	GetScene()->AddGameObject(attackObj);

	// ���Ÿ� ���ݻ���  
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

	// ������ ���� ����
	auto arcAttack = attackObj->GetComponent<PlantAOEAttack>();
	arcAttack->SetmaxRange(mAttackRange);
	arcAttack->SetStartPosition(transform->GetWorldPosition() + offset);
	arcAttack->SetTargetPosition(mTarget->GetComponent<fq::game_module::Transform>()->GetWorldPosition());

	// ���� ����
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	AttackInfo info{};
	info.attacker = attackObj.get();
	info.damage = mAttackPower;
	info.bIsInfinite = false;
	info.remainingAttackCount = 0b11111111;
	attackComponent->Set(info);

	// ���� ��Ÿ�� ����ó��
	mAttackElapsedTime = mAttackCoolTime;

	GetScene()->AddGameObject(attackObj);
	arcAttack->DetermineArrivalTime();
}

void fq::client::PlantMonster::OnUpdate(float dt)
{
	// Target ���� Ȯ��
	if (mTarget)
	{
		if (mTarget->IsDestroyed())
		{
			auto playerOrNull = mTarget->GetComponent<Player>();

			if (playerOrNull == nullptr)
			{
				SetTarget(nullptr);
			}
			else
			{
				auto dummyOrNull = playerOrNull->CreateDummyOrNull();
				SetTarget(dummyOrNull);
			}
		}
	}

	// ���� ��Ÿ�� 
	mAttackElapsedTime = std::max(mAttackElapsedTime - dt, 0.f);

	// ���� Ÿ���� ���� Ÿ������ üũ
	if (mIsDummyTarget)
	{
		mDummyTraceElapsedTime += dt;

		if (mCurrentDummyTraceDurationTime < mDummyTraceElapsedTime)
		{
			SetTarget(nullptr);
		}
	}
}

void fq::client::PlantMonster::OnTriggerEnter(const game_module::Collision& collision)
{
	// �÷��̾� �ǰ� ó�� 
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		auto playerAttack = collision.other->GetComponent<client::Attack>();
		if (playerAttack->ProcessAttack())
		{
			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = playerAttack->GetAttackPower();
			mHp -= attackPower;

			if (mMonsterHpUI)
				mMonsterHpUI->DecreaseHp(attackPower / mMaxHp);

			// Ÿ���� �ڽ��� ���� ������� �ٲߴϴ� 
			SetTarget(playerAttack->GetAttacker());

			// �ǰ� ���� ���
			playerAttack->PlayHitSound();

			// PlantMonster ��� ó�� 
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

			// ����Ʈ ����
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

void fq::client::PlantMonster::destroySocketCollider()
{
	// ������ ������ �ִ� �ڽ� ������Ʈ�� �ڽ� �ݶ��̴� ����
	for (auto childObject : GetGameObject()->GetChildren())
	{
		auto socket = childObject->GetComponent<fq::game_module::Socket>();

		if (socket)
			childObject->RemoveComponent<fq::game_module::BoxCollider>();
	}
}

void fq::client::PlantMonster::SetTarget(fq::game_module::GameObject* target)
{
	if (mAnimator == nullptr)
	{
		mAnimator = GetComponent<game_module::Animator>();
		assert(mAnimator);
	}

	// Target�� ����� ���
	if (target == nullptr)
	{
		mTarget = nullptr;
		mAnimator->SetParameterBoolean("HasTarget", false);
		mIsDummyTarget = false;
		return;
	}

	// ���� Ÿ������ üũ
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

void fq::client::PlantMonster::LookAtTarget()
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

	auto transform = GetComponent<game_module::Transform>();
	auto targetT = mTarget->GetComponent<game_module::Transform>();

	auto targetPos = targetT->GetWorldPosition();
	auto myPos = transform->GetWorldPosition();
	auto currentRotation = transform->GetWorldRotation();
	auto directV = targetPos - myPos;
	directV.y = 0.f;
	directV.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = currentRotation;

	// UpVector�� �������� ��쿡 ���� ���� ó�� 
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

	// ���� ����� �÷��̾ ����
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

void fq::client::PlantMonster::DestroyMonsterHPUI()
{
	if (mMonsterHpUI)
	{
		GetScene()->DestroyGameObject(mMonsterHpUI->GetGameObject());
		mMonsterHpUI = nullptr;
	}
	else
	{
		spdlog::warn("PlantMonster has not monster ui");
	}
}

void fq::client::PlantMonster::HitArrow(fq::game_module::GameObject* object)
{
	auto playerAttack = object->GetComponent<client::Attack>();
	
	// �̹� �������� ���� ������Ʈ��� �Լ� ����
	auto attackObject = mArrowAttackObject.find(object->GetID());
	if (attackObject == mArrowAttackObject.end())
	{
		mArrowAttackObject.insert(object->GetID());
	}
	else
	{
		return;
	}

	if (playerAttack->ProcessAttack())
	{
		mAnimator->SetParameterTrigger("OnHit");
		float attackPower = playerAttack->GetAttackPower();
		mHp -= attackPower;

		if (mMonsterHpUI)
			mMonsterHpUI->DecreaseHp(attackPower / mMaxHp);

		// Ÿ���� �ڽ��� ���� ������� �ٲߴϴ� 
		SetTarget(playerAttack->GetAttacker());

		// �ǰ� ���� ���
		playerAttack->PlayHitSound();

		// PlantMonster ��� ó�� 
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

		// ����Ʈ ����
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}
}