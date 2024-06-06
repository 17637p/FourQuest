#include "MonsterAttack.h"
#include "Monster.h"

fq::client::MonsterAttack::MonsterAttack()
{

}

fq::client::MonsterAttack::~MonsterAttack()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterAttack::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterAttack>(*this);
}

void fq::client::MonsterAttack::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	rotateTowards(animator);
	makeAttackRangeCollider();
	mIsStartAttack = false;
}

void fq::client::MonsterAttack::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{
	Monster* monster = animator.GetComponent<Monster>();

	/// 공격 실행
	if (mIsStartAttack)
	{
		// 처리하고
		monster->SetTarget(nullptr);
		animator.SetParameterTrigger("OnIdle");
	}

	// Todo: deltaTime으로 바꿀것
	mWaitTime += 0.0166666f;
	if (mWaitTime > monster->GetAttackWaitTime())
	{
		mIsStartAttack = true;
	}
}

void fq::client::MonsterAttack::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}

void fq::client::MonsterAttack::rotateTowards(fq::game_module::Animator& animator)
{
	Monster* monster = animator.GetComponent<Monster>();

	// 플레이어 위치
	DirectX::SimpleMath::Vector3 playerPosition = monster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	// 내위치
	auto myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	// 빼기
	DirectX::SimpleMath::Vector3 directionVector = myPosition - playerPosition;
	DirectX::SimpleMath::Vector3 curVector = getDirectionVectorFromQuaternion(myTransform->GetLocalRotation());

	directionVector.Normalize();
	curVector.Normalize();

	DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::FromToRotation(curVector, directionVector);

	myTransform->SetLocalRotation(myTransform->GetLocalRotation() * directionQuaternion);
}

DirectX::SimpleMath::Vector3 fq::client::MonsterAttack::getDirectionVectorFromQuaternion(const DirectX::SimpleMath::Quaternion& quaternion)
{
	DirectX::SimpleMath::Vector3 baseDirection{ 0, 0, 1 };

	DirectX::SimpleMath::Vector3 directionVector =
		DirectX::SimpleMath::Vector3::Transform(baseDirection, quaternion);

	return directionVector;
}

void fq::client::MonsterAttack::makeAttackRangeCollider()
{

}
