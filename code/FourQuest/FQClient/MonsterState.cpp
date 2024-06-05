#include "MonsterState.h"

#include "Monster.h"
#include "PlayerMovement.h"

#pragma region Idle
fq::client::MonsterIdleState::MonsterIdleState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{monster}
{

}

fq::client::MonsterIdleState::~MonsterIdleState()
{

}

void fq::client::MonsterIdleState::OnStateEnter()
{

}

void fq::client::MonsterIdleState::OnStateUpdate(float deltaTime)
{
	if (mMonster->GetTarget() == nullptr)
	{
		mMonster->ChangeState("Find");
	}
	else
	{
		// Todo: 데미지 받았을 때 변수 설정 
		if (mMonster->GetIsDamagaed())
		{
			mMonster->ChangeState("Damaged");
		}
		else
		{
			float targetDist = mMonster->CalculateDistance(*mMonster->GetTarget());

			if (targetDist < mMonster->GetTargetAttackRange())
			{
				mMonster->ChangeState("AttatchPreparation");
			}
			else
			{
				mMonster->ChangeState("Chase");
			}
		}
	}
}

void fq::client::MonsterIdleState::OnStateExit()
{

}
#pragma endregion Idle

#pragma region Find
fq::client::MonsterFindState::MonsterFindState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{

}

fq::client::MonsterFindState::~MonsterFindState()
{

}

void fq::client::MonsterFindState::OnStateEnter()
{
	auto componentView = mMonster->GetScene()->GetComponentView<PlayerMovement>();

	float distMin = FLT_MAX;

	for (auto& player : componentView)
	{
		float dist = mMonster->CalculateDistance(player);

		if (distMin > dist)
		{
			distMin = dist;
			mMonster->SetTarget(&player);
		}
	}

	mMonster->ChangeState("Idle");
}

void fq::client::MonsterFindState::OnStateUpdate(float deltaTime)
{
	
}

void fq::client::MonsterFindState::OnStateExit()
{

}
#pragma endregion Find

#pragma region Chase
fq::client::MonsterChaseState::MonsterChaseState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster },
	mMoveDistance(0)
{

}

fq::client::MonsterChaseState::~MonsterChaseState()
{

}

void fq::client::MonsterChaseState::OnStateEnter()
{
	mMoveDistance = 0;
	rotateTowards();
}

void fq::client::MonsterChaseState::OnStateUpdate(float deltaTime)
{
	float moveDist = deltaTime * mMonster->GetMoveSpeed();
	MoveToTarget(moveDist);
	mMoveDistance += moveDist;

	if (mMoveDistance > mMonster->GetChaseDistance())
	{
		mMonster->ChangeState("Idle");
	}
}

void fq::client::MonsterChaseState::OnStateExit()
{

}

void fq::client::MonsterChaseState::MoveToTarget(float dist)
{
	DirectX::SimpleMath::Vector3 playerPosition = mMonster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	auto myTransform = mMonster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();

	DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
	directionVector.Normalize();

	myTransform->SetLocalPosition(directionVector * dist + myPosition);
;}

DirectX::SimpleMath::Vector3 fq::client::MonsterChaseState::getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion)
{
	DirectX::SimpleMath::Vector3 baseDirection{ 0, 0, 1 };

	DirectX::SimpleMath::Vector3 directionVector = 
		DirectX::SimpleMath::Vector3::Transform(baseDirection, quaternion);

	return directionVector;
}

void fq::client::MonsterChaseState::rotateTowards()
{
	// 플레이어 위치
	DirectX::SimpleMath::Vector3 playerPosition = mMonster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	// 내위치
	auto myTransform = mMonster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	// 빼기
	DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
	DirectX::SimpleMath::Vector3 curVector = getDirectionVectorFromQuaternion(myTransform->GetLocalRotation());

	directionVector.Normalize();
	curVector.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::FromToRotation(curVector, directionVector);

	myTransform->SetLocalRotation(myTransform->GetLocalRotation() * directionQuaternion);
}

#pragma endregion Chase

#pragma region Damaged
fq::client::MonsterDamagedState::MonsterDamagedState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{

}

fq::client::MonsterDamagedState::~MonsterDamagedState()
{

}

void fq::client::MonsterDamagedState::OnStateEnter()
{
	mMonster->GetAnimator()->SetParameterTrigger("OnDamaged");
	mMonster->SetHP(mMonster->GetHP() - mMonster->GetDamaged());
	if (mMonster->GetHP() <= 0)
	{
		mMonster->ChangeState("Death");
	}
	else
	{
		mMonster->SetTarget(mMonster->GetLastAttacker());
		mMonster->ChangeState("Idle");
	}
}

void fq::client::MonsterDamagedState::OnStateUpdate(float deltaTime)
{

}

void fq::client::MonsterDamagedState::OnStateExit()
{

}
#pragma endregion Damaged

#pragma region Death
fq::client::MonsterDeathState::MonsterDeathState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{

}

fq::client::MonsterDeathState::~MonsterDeathState()
{

}

void fq::client::MonsterDeathState::OnStateEnter()
{
	mMonster->GetAnimator()->SetParameterTrigger("OnDeath");
}

void fq::client::MonsterDeathState::OnStateUpdate(float deltaTime)
{
	fq::game_module::GameObject* monsterObject = mMonster->GetGameObject();
	// Todo: Monster 애니메이션 재생 완료 되었는지 확인 한 다음 
	// Animation 종료 되었는지 확인하는 이벤트
	std::string curAnimationName = mMonster->GetAnimator()->GetController().GetCurrentStateName();
	if (curAnimationName != "Death")
	{
		mMonster->GetScene()->DestroyGameObject(monsterObject);
	}
}

void fq::client::MonsterDeathState::OnStateExit()
{

}
#pragma endregion Death

#pragma region AttatchPreparation
fq::client::MonsterAttatchPreparationState::MonsterAttatchPreparationState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{
	mMonster->GetAnimator()->SetParameterTrigger("OnAttack");
}

fq::client::MonsterAttatchPreparationState::~MonsterAttatchPreparationState()
{

}

void fq::client::MonsterAttatchPreparationState::OnStateEnter()
{
	rotateTowards();
	makeAttackRangeCollider();
}

void fq::client::MonsterAttatchPreparationState::OnStateUpdate(float deltaTime)
{

}

void fq::client::MonsterAttatchPreparationState::OnStateExit()
{

}

void fq::client::MonsterAttatchPreparationState::rotateTowards()
{
	// 플레이어 위치
	DirectX::SimpleMath::Vector3 playerPosition = mMonster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	// 내위치
	auto myTransform = mMonster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	// 빼기
	DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
	DirectX::SimpleMath::Vector3 curVector = getDirectionVectorFromQuaternion(myTransform->GetLocalRotation());

	directionVector.Normalize();
	curVector.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::FromToRotation(curVector, directionVector);

	myTransform->SetLocalRotation(myTransform->GetLocalRotation() * directionQuaternion);
}

DirectX::SimpleMath::Vector3 fq::client::MonsterAttatchPreparationState::getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion)
{
	DirectX::SimpleMath::Vector3 baseDirection{ 0, 0, 1 };

	DirectX::SimpleMath::Vector3 directionVector =
		DirectX::SimpleMath::Vector3::Transform(baseDirection, quaternion);

	return directionVector;
}

void fq::client::MonsterAttatchPreparationState::makeAttackRangeCollider()
{

}
#pragma endregion AttatchPreparation

#pragma region AttatchIdle
fq::client::MonsterAttatchIdleState::MonsterAttatchIdleState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{
	
}

fq::client::MonsterAttatchIdleState::~MonsterAttatchIdleState()
{

}

void fq::client::MonsterAttatchIdleState::OnStateEnter()
{
	
}

void fq::client::MonsterAttatchIdleState::OnStateUpdate(float deltaTime)
{
	mWaitTime += deltaTime;
	if (mWaitTime > mMonster->GetAttackWaitTime())
	{
		mMonster->ChangeState("AttatchExecution");
	}
}

void fq::client::MonsterAttatchIdleState::OnStateExit()
{

}

#pragma endregion AttatchIdle

#pragma region AttatchExecution
fq::client::MonsterAttatchExecutionState::MonsterAttatchExecutionState(std::shared_ptr<Monster> monster)
	:MonsterBaseState{ monster }
{

}

fq::client::MonsterAttatchExecutionState::~MonsterAttatchExecutionState()
{

}

void fq::client::MonsterAttatchExecutionState::OnStateEnter()
{

}

void fq::client::MonsterAttatchExecutionState::OnStateUpdate(float deltaTime)
{

	mMonster->SetTarget(nullptr);
	mMonster->ChangeState("Idle");
}

void fq::client::MonsterAttatchExecutionState::OnStateExit()
{

}
#pragma endregion AttatchExecution
