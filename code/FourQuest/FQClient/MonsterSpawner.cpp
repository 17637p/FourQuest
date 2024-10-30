#define  NOMINMAX
#include "MonsterSpawner.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/NavigationAgent.h"
#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "Attack.h"
#include "MonsterGroup.h"
#include "MeleeMonster.h"
#include "HpBar.h"

#include <spdlog/spdlog.h>
#include "Player.h"
#include "PlayerInfoVariable.h"
#include "MonsterHP.h"

fq::client::MonsterSpawner::MonsterSpawner()
	:mSpawnCoolTime(10.f)
	, mSpawnElapsedTime(0.f)
	, mHp(1000.f)
	, mMaxHp(1000.f)
	, mSpawnOffset(2.f)
	, mHitElapsedTime(0.f)
	, mbIsSpawnState(false)
	, mTransform(nullptr)
	, mMonsterGroup(nullptr)
	, mAnimator(nullptr)
	, mMonsterHp(nullptr)
	, mbOnHitRimLight(false)
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
	if (agent)
	{
		agent->SetAgentState();
	}

	if (mbOnHitRimLight)
	{
		mHitElapsedTime += dt;
		constexpr float HitRimLightTime = 0.15f;

		if (mHitElapsedTime >= HitRimLightTime)
		{
			mbOnHitRimLight = false;
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = false;

			for (auto child : GetGameObject()->GetChildren())
			{
				auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

				if (skeletalMesh != nullptr)
				{
					fq::graphics::MaterialInstanceInfo info;
					info.bUseRimLight = false;
					skeletalMesh->SetMaterialInstanceInfo(info);
				}
			}
		}
	}
}

void fq::client::MonsterSpawner::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mTransform = GetComponent<game_module::Transform>();
	mMonsterGroup = GetComponent<MonsterGroup>();

	for (auto child : mTransform->GetChildren())
	{
		auto hpBar = child->GetComponent<MonsterHP>();
		if (hpBar)
		{
			mMonsterHp = hpBar;
		}
	}


	mMaxHp = mHp;
	mSpawnElapsedTime = mSpawnCoolTime;

	auto agent = GetComponent<game_module::NavigationAgent>();
	if (agent)
	{
		agent->SetSpeed(0.f);
		agent->SetAcceleration(0.f);
		agent->SetRadius(3.f);
		agent->SetSyncRotationWithMovementDirection(false);
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
	monsterT->SetWorldPosition(monsterPos);

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
			mMonsterHp->DecreaseHp(damage / mMaxHp);

			if (!mbIsSpawnState)
			{
				mAnimator->SetParameterTrigger("OnHit");
			}

			// �ǰ� ������Ʈ ���� 
			onHitRimLight();

			// ���� �������� Ÿ�� ����
			mMonsterGroup->SetTarget(playerAttack->GetAttacker());

			// �ǰ� ���� ���
			playerAttack->PlayHitSound();

			// ������ ��� ó�� 
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
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "Spawner_Death", false , fq::sound::EChannel::SE });
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

void fq::client::MonsterSpawner::HitArrow(fq::game_module::GameObject* object)
{
	auto playerAttack = object->GetComponent<Attack>();

	if (playerAttack->ProcessAttack())
	{
		float damage = playerAttack->GetAttackPower();
		mHp -= damage;
		GetComponent<HpBar>()->DecreaseHp(damage / mMaxHp);

		if (!mbIsSpawnState)
		{
			mAnimator->SetParameterTrigger("OnHit");
		}

		// ���� �������� Ÿ�� ����
		mMonsterGroup->SetTarget(playerAttack->GetAttacker());

		// �ǰ� ���� ���
		playerAttack->PlayHitSound();

		// ������ ��� ó�� 
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
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "Spawner_Death", false , fq::sound::EChannel::SE });
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

void fq::client::MonsterSpawner::DetectPlayer(game_module::GameObject* target)
{
	mMonsterGroup->SetTarget(target);
	mAnimator->SetParameterTrigger("Detect");
}

void fq::client::MonsterSpawner::onHitRimLight()
{
	mHitElapsedTime = 0.f;

	if (mbOnHitRimLight)
	{
		return;
	}

	mbOnHitRimLight = true;
	for (auto child : GetGameObject()->GetChildren())
	{
		auto skeletalMesh = child->GetComponent<game_module::SkinnedMeshRenderer>();

		if (skeletalMesh != nullptr)
		{
			fq::graphics::MaterialInstanceInfo info;
			info.bUseRimLight = true;
			info.RimLightColor = DirectX::SimpleMath::Color{ 1.f, 0.f, 0.f, 1.f };
			info.RimPow = 0.f;
			info.RimIntensity = 0.5f;

			skeletalMesh->SetMaterialInstanceInfo(info);
		}
	}
}

