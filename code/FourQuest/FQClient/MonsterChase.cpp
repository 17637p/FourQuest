#include "MonsterChase.h"
#include "Monster.h"

#include "../FQGameModule/NavigationAgent.h"

fq::client::MonsterChase::MonsterChase()
	:mMoveDistance(0)
{
}

fq::client::MonsterChase::~MonsterChase()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterChase::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterChase>(*this);
}

void fq::client::MonsterChase::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	mMoveDistance = 0;
}

void fq::client::MonsterChase::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	Monster* monster = animator.GetComponent<Monster>();
	fq::game_module::NavigationAgent* agent = animator.GetComponent<fq::game_module::NavigationAgent>();

	if (monster->GetIsDamaged())
	{
		animator.SetParameterTrigger("OnDamaged");
		agent->Stop();
		return;
	}

	fq::game_module::GameObject* target = monster->GetTarget().get();
	if (target->IsDestroyed())
	{
		animator.SetParameterTrigger("OnIdle");
		agent->Stop();
		monster->SetTarget(nullptr);

		return;
	}

	// Todo: dt로 바꾸기 
	float moveDist = dt * monster->GetMoveSpeed();
	rotateToTarget(animator);
	MoveToTarget(animator, moveDist);
	mMoveDistance += 0;

 	game_module::GameObject& object = *monster->GetTarget();

	// 타겟과의 거리가 가까울 때
	float targetDist = monster->CalculateDistanceTarget(monster->GetTarget().get());

	if (targetDist < monster->GetTargetAttackRange())
	{
		animator.SetParameterTrigger("OnIdle");
		agent->Stop();
		monster->SetTarget(nullptr);

		return;
	}

	if (mMoveDistance > monster->GetChaseDistance())
	{
		animator.SetParameterTrigger("OnIdle");
		agent->Stop();
		monster->SetTarget(nullptr);

		return;
	}
}

void fq::client::MonsterChase::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}

void fq::client::MonsterChase::MoveToTarget(fq::game_module::Animator& animator, float dist)
{
	Monster* monster = animator.GetComponent<Monster>();
	fq::game_module::NavigationAgent* agent = animator.GetComponent<fq::game_module::NavigationAgent>();

	fq::game_module::GameObject* target = monster->GetTarget().get();
	
	DirectX::SimpleMath::Vector3 targetPosition = target->GetComponent<fq::game_module::Transform>()->GetWorldPosition();

	//fq::game_module::Transform* myTransform = monster->GetComponent<fq::game_module::Transform>();
	//DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();

	agent->MoveTo(targetPosition);
	//DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
	//directionVector.Normalize();
	//
	//myTransform->SetLocalPosition(directionVector * dist + myPosition);
}

void fq::client::MonsterChase::rotateToTarget(fq::game_module::Animator& animator)
{
	Monster* monster = animator.GetComponent<Monster>();

	// 플레이어 위치
	fq::game_module::GameObject* target = monster->GetTarget().get();
	fq::game_module::NavigationAgent* agent = animator.GetComponent<fq::game_module::NavigationAgent>();

	DirectX::SimpleMath::Vector3 targetPosition = target->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	
	// 내위치
	fq::game_module::Transform* myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	
	// 빼기
	DirectX::SimpleMath::Vector3 directionVector = targetPosition - myPosition;
	directionVector.y = 0;

	directionVector.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion;
	if (directionVector == DirectX::SimpleMath::Vector3::Backward)
	{
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directionVector, { 0, -1, 0 });
	}
	else
	{
		directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directionVector, { 0, 1, 0 });
	}

	myTransform->SetLocalRotation(directionQuaternion);
}
