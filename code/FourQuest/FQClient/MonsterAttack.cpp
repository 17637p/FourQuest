#include "MonsterAttack.h"
#include "Monster.h"
#include "Attack.h"

fq::client::MonsterAttack::MonsterAttack()
	:mWaitTime(0),
	mIsTransition(false)
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
	// Target 방향 회전
	rotateToTarget(animator);

	// 공격 대기 시간 설정
	mWaitTime = 0;
	mIsTransition = false;
}

void fq::client::MonsterAttack::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{
	Monster* monster = animator.GetComponent<Monster>();

	// 공격 대기 상태일 때 피격 당하면 
	if (monster->GetIsDamaged())
	{
		animator.SetParameterTrigger("OnDamaged");
		mIsTransition = true;
		return;
	}

	mWaitTime += dt;
	if (mWaitTime > monster->GetAttackWaitTime())
	{
		animator.SetParameterTrigger("OnIdle");

		if (mIsTransition)
		{
			return;
		}

		Monster* monster = animator.GetComponent<Monster>();
		
		attack(animator);

		// 공격 이후 설정 
		monster->SetTarget(nullptr);
		mIsTransition = true;
	}
}

void fq::client::MonsterAttack::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
}

void fq::client::MonsterAttack::rotateToTarget(fq::game_module::Animator& animator)
{
	Monster* monster = animator.GetComponent<Monster>();

	// 플레이어 위치
	fq::game_module::GameObject* target = monster->GetTarget();
	if (target->IsDestroyed())
	{
		animator.SetParameterTrigger("OnIdle");
		monster->SetTarget(nullptr);
		return;
	}
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

void fq::client::MonsterAttack::attack(fq::game_module::Animator& animator)
{
	Monster* monster = animator.GetComponent<Monster>();
	fq::game_module::Transform* monsterTransform = monster->GetComponent<fq::game_module::Transform>();

	// Attack 수치 설정 (공격력, Attackter, 공격 콜라이더 크기, 태그, 회전, 위치)
	auto instance = monster->GetScene()->GetPrefabManager()->InstantiatePrefabResoure(monster->GetAttack());
	auto& object = *(instance.begin());
	fq::game_module::Transform* transform = object->GetComponent<fq::game_module::Transform>();
	fq::game_module::BoxCollider* collider = object->GetComponent<fq::game_module::BoxCollider>();
	fq::client::Attack* attack = object->GetComponent<fq::client::Attack>();

	// 공격 위한 세팅 
	attack->SetAttackPower(20);
	attack->SetAttacker(monster->GetGameObject());
	collider->SetExtent({ 0.4f, 0.2f, 0.4f });
	object->SetTag(fq::game_module::ETag::MonsterAttack);

	/// Transform(회전, 위치 설정)
	// 몬스터 회전 값 가져와서 회전 
	DirectX::SimpleMath::Quaternion monsterRoatation = monsterTransform->GetWorldRotation();
	object->GetComponent<fq::game_module::Transform>()->SetLocalRotation({
		monsterRoatation });
	// 콜라이더 생성 위치 벡터
	DirectX::SimpleMath::Vector3 createVector = { 0, 1.25f, -0.5f };

	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(monsterRoatation);
	createVector = DirectX::SimpleMath::Vector3::Transform(createVector, rotationMatrix);

	object->GetComponent<fq::game_module::Transform>()->SetLocalPosition({
		monsterTransform->GetWorldPosition().x + createVector.x,
		monsterTransform->GetWorldPosition().y + createVector.y,
		monsterTransform->GetWorldPosition().z + createVector.z });

	// 생성
	monster->GetScene()->AddGameObject(object);
}
