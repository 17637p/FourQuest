#include "BossMonsterPreJumpState.h"
#include "BossMonster.h"
#include "GameManager.h"
#include "Player.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/NavigationAgent.h"

fq::client::BossMonsterPreJumpState::BossMonsterPreJumpState()
	: mWaitingTime(0.5f)
	, mElapsed(0.f)
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::BossMonsterPreJumpState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<BossMonsterPreJumpState>(*this);
}

void fq::client::BossMonsterPreJumpState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	auto naviAgent = animator.GetComponent<game_module::NavigationAgent>();

	if (naviAgent)
	{
		naviAgent->Stop();
	}

	auto boss = animator.GetComponent<BossMonster>();
	DirectX::SimpleMath::Vector3 defulatJumpPosition;

	if (boss != nullptr)
	{
		boss->StepBack();
		defulatJumpPosition = boss->GetDefaultJumpPosition();
	}

	mElapsed = 0.f;

	selectJumpPosition(animator);

	mDecalEffectObject = boss->EmitJumpDecalEffect();
}

void fq::client::BossMonsterPreJumpState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	auto boss = animator.GetComponent<BossMonster>();

	// 시간 누적
	mElapsed += dt;

	if (mWaitingTime < mElapsed)
	{
		// set triger;
		boss->ResetJumpCoolTime();
		animator.SetParameterBoolean("OnJumpWaiting", false);
		return;
	}
	if (mTargetCaptureTime > mElapsed)
	{
		selectJumpPosition(animator);

		if (mDecalEffectObject != nullptr)
		{
			mDecalEffectObject->GetTransform()->SetWorldPosition(boss->GetJumpPosition());
		}
	}
}

void fq::client::BossMonsterPreJumpState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	if (mDecalEffectObject != nullptr)
	{
		animator.GetScene()->DestroyGameObject(mDecalEffectObject.get());
		mDecalEffectObject = nullptr;
	}

	animator.SetParameterBoolean("OnJump", true);
}

bool fq::client::BossMonsterPreJumpState::existBetweenWallObject(game_module::Scene* scene, const DirectX::SimpleMath::Vector3& origin, const DirectX::SimpleMath::Vector3& target, game_module::ETag tag)
{

	DirectX::SimpleMath::Vector3 direction = target - origin;
	float distance = direction.Distance(origin, target);
	bool bUseDebugDraw = true;
	bool isStatic = true;
	direction.Normalize();

	fq::event::RayCast::ResultData data;

	scene->GetEventManager()->FireEvent<fq::event::RayCast>(
		fq::event::RayCast {origin, direction, distance, tag, & data, bUseDebugDraw, isStatic}
	);

	for (auto hitObject : data.hitObjects)
	{
		if (hitObject->GetTag() == game_module::ETag::Wall)
		{
			return true;
		}
	}

	return false;
}

void fq::client::BossMonsterPreJumpState::selectJumpPosition(game_module::Animator& animator)
{
	auto boss = animator.GetComponent<BossMonster>();
	DirectX::SimpleMath::Vector3 defulatJumpPosition;

	if (boss != nullptr)
	{
		defulatJumpPosition = boss->GetDefaultJumpPosition();
	}

	auto worldPosition = boss->GetTransform()->GetWorldPosition();

	// target 사이에 점프가 가능한지 체크
	if (boss->GetTarget() != nullptr)
	{
		auto targetWorldPosition = boss->GetTarget()->GetTransform()->GetWorldPosition();

		if (!existBetweenWallObject(animator.GetScene(), worldPosition, targetWorldPosition, animator.GetGameObject()->GetTag()))
		{
			boss->SetJumpPosition(targetWorldPosition);
			boss->HomingTarget();
			return;
		}
	}

	// 게임 매니저 관련 예외 처리
	auto gameManager = animator.GetScene()->GetObjectByName("GameManager");

	if (gameManager == nullptr)
	{
		boss->SetJumpPosition(defulatJumpPosition);
		return;
	}

	auto gameManagerComp = gameManager->GetComponent<GameManager>();

	if (gameManagerComp == nullptr)
	{
		boss->SetJumpPosition(defulatJumpPosition);
		return;
	}

	// 다른 플레이어 사이에 벽이 없는지 체크
	for (const auto& player : gameManagerComp->GetPlayers())
	{
		if (player->GetComponent<Player>() == nullptr)
		{
			continue;
		}

		auto targetWorldPosition = player->GetTransform()->GetWorldPosition();

		if (!existBetweenWallObject(animator.GetScene(), worldPosition, targetWorldPosition, animator.GetGameObject()->GetTag()))
		{
			boss->SetJumpPosition(targetWorldPosition);
			boss->SetTarget(player.get());
			boss->HomingTarget();
			return;
		}
	}

	// 설정한 게 없다면 여기로
	auto transform = animator.GetComponent<fq::game_module::Transform>();
	auto objectPos = transform->GetWorldPosition();
	auto objectRotation = transform->GetWorldRotation();
	auto direction = defulatJumpPosition - objectPos;
	direction.y = 0.f;
	direction.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = objectRotation;

	// UpVector가 뒤집히는 경우에 대한 예외 처리 
	if (direction.z >= 1.f)
	{
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation({ 0.f,0.f,1.f }, { 0.f, -1.f, 0.f });
	}
	else if (direction != DirectX::SimpleMath::Vector3::Zero)
	{
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(direction, { 0.f,1.f,0.f });
	}
	directionQuaternion.Normalize();
	transform->SetWorldRotation(directionQuaternion);
}
