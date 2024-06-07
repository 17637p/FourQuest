#include "MonsterChase.h"
#include "Monster.h"

fq::client::MonsterChase::MonsterChase()
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

	// Todo: dt로 바꾸기 
	float moveDist = 0.166666f * monster->GetMoveSpeed();
	rotateTowards(animator);
	MoveToTarget(animator, moveDist);
	mMoveDistance += moveDist;

	// 타겟과의 거리가 가까울 때
	float targetDist = monster->CalculateDistance(*monster->GetTarget());

	if (targetDist < monster->GetTargetAttackRange())
	{
		animator.SetParameterTrigger("OnIdle");
	}

	if (mMoveDistance > monster->GetChaseDistance())
	{
		animator.SetParameterTrigger("OnIdle");
	}
}

void fq::client::MonsterChase::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}

void fq::client::MonsterChase::MoveToTarget(fq::game_module::Animator& animator, float dist)
{
	Monster* monster = animator.GetComponent<Monster>();

	DirectX::SimpleMath::Vector3 playerPosition = monster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	auto myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();

	DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
	directionVector.Normalize();

	myTransform->SetLocalPosition(directionVector * dist + myPosition);
}

void fq::client::MonsterChase::rotateTowards(fq::game_module::Animator& animator)
{
	Monster* monster = animator.GetComponent<Monster>();

	// 플레이어 위치
	DirectX::SimpleMath::Vector3 playerPosition = monster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	// 내위치
	auto myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	// 빼기
	DirectX::SimpleMath::Vector3 directionVector = playerPosition - myPosition;
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

DirectX::SimpleMath::Vector3 fq::client::MonsterChase::getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion)
{
	DirectX::SimpleMath::Vector3 baseDirection{ 0, 0, 1 };

	DirectX::SimpleMath::Vector3 directionVector =
		DirectX::SimpleMath::Vector3::Transform(baseDirection, quaternion);

	return directionVector;
}
