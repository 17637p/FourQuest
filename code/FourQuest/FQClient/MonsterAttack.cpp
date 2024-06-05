#include "MonsterAttack.h"
#include "Monster.h"
#include "Attack.h"

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
	mIsTransition = false;
}

void fq::client::MonsterAttack::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	Monster* monster = animator.GetComponent<Monster>();
	auto monsterTransform = monster->GetComponent<fq::game_module::Transform>();

	/// 공격 실행
	if (mIsStartAttack && !mIsTransition)
	{
		// 처리하고
		auto instance = monster->GetScene()->GetPrefabManager()->InstantiatePrefabResoure(monster->GetAttack());
		auto& object = *(instance.begin());
		auto transform = object->GetComponent<fq::game_module::Transform>();
		auto collider = object->GetComponent<fq::game_module::BoxCollider>();
		auto attack = object->GetComponent<fq::client::Attack>();

		attack->SetAttackPower(20);
		collider->SetExtent({ 0.4f, 0.2f, 0.2f });
		
		DirectX::SimpleMath::Vector3 createVector = { 0, 1.25f, 0.5f };

		// 플레이어 위치
		DirectX::SimpleMath::Vector3 playerPosition = monster->GetTarget()->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
		// 내위치
		auto monsterTransform = monster->GetComponent<fq::game_module::Transform>();
		DirectX::SimpleMath::Vector3 monsterPostion = monsterTransform->GetWorldPosition();
		// 빼기
		DirectX::SimpleMath::Vector3 directionVector = monsterPostion - playerPosition;
		directionVector.Normalize();

		createVector = {createVector.x * directionVector.x + 
		createVector.y * directionVector.y,
		createVector.z * directionVector.z };

		object->GetComponent<fq::game_module::Transform>()->SetLocalPosition({ 
			monsterTransform->GetWorldPosition().x + createVector.x,
			monsterTransform->GetWorldPosition().y + createVector.y,
			monsterTransform->GetWorldPosition().z + createVector.z });

		object->GetComponent<fq::game_module::Transform>()->SetLocalRotation({
			monsterTransform->GetWorldRotation()});
		object->SetTag(fq::game_module::ETag::Alien);

		monster->GetScene()->AddGameObject(object);

		monster->SetTarget(nullptr);
		animator.SetParameterTrigger("OnIdle");

		mIsTransition = true;
		return;
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
