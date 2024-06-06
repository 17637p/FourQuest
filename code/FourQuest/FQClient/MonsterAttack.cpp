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

	/// ���� ����
	if (mIsStartAttack)
	{
		// ó���ϰ�
		monster->SetTarget(nullptr);
		animator.SetParameterTrigger("OnIdle");
	}

	// Todo: deltaTime���� �ٲܰ�
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

	// �÷��̾� ��ġ
	DirectX::SimpleMath::Vector3 playerPosition = monster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	// ����ġ
	auto myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	// ����
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
