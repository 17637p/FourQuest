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
	// Target ���� ȸ��
	rotateToTarget(animator);

	// ���� ��� �ð� ����
	mWaitTime = 0;
	mIsTransition = false;
}

void fq::client::MonsterAttack::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state, float dt)
{
	Monster* monster = animator.GetComponent<Monster>();

	// ���� ��� ������ �� �ǰ� ���ϸ� 
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

		// ���� ���� ���� 
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

	// �÷��̾� ��ġ
	fq::game_module::GameObject* target = monster->GetTarget();
	if (target->IsDestroyed())
	{
		animator.SetParameterTrigger("OnIdle");
		monster->SetTarget(nullptr);
		return;
	}
	DirectX::SimpleMath::Vector3 targetPosition = target->GetComponent<fq::game_module::Transform>()->GetWorldPosition();
	
	// ����ġ
	fq::game_module::Transform* myTransform = monster->GetComponent<fq::game_module::Transform>();
	DirectX::SimpleMath::Vector3 myPosition = myTransform->GetWorldPosition();
	
	// ����
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

	// Attack ��ġ ���� (���ݷ�, Attackter, ���� �ݶ��̴� ũ��, �±�, ȸ��, ��ġ)
	auto instance = monster->GetScene()->GetPrefabManager()->InstantiatePrefabResoure(monster->GetAttack());
	auto& object = *(instance.begin());
	fq::game_module::Transform* transform = object->GetComponent<fq::game_module::Transform>();
	fq::game_module::BoxCollider* collider = object->GetComponent<fq::game_module::BoxCollider>();
	fq::client::Attack* attack = object->GetComponent<fq::client::Attack>();

	// ���� ���� ���� 
	attack->SetAttackPower(20);
	attack->SetAttacker(monster->GetGameObject());
	collider->SetExtent({ 0.4f, 0.2f, 0.4f });
	object->SetTag(fq::game_module::ETag::MonsterAttack);

	/// Transform(ȸ��, ��ġ ����)
	// ���� ȸ�� �� �����ͼ� ȸ�� 
	DirectX::SimpleMath::Quaternion monsterRoatation = monsterTransform->GetWorldRotation();
	object->GetComponent<fq::game_module::Transform>()->SetLocalRotation({
		monsterRoatation });
	// �ݶ��̴� ���� ��ġ ����
	DirectX::SimpleMath::Vector3 createVector = { 0, 1.25f, -0.5f };

	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(monsterRoatation);
	createVector = DirectX::SimpleMath::Vector3::Transform(createVector, rotationMatrix);

	object->GetComponent<fq::game_module::Transform>()->SetLocalPosition({
		monsterTransform->GetWorldPosition().x + createVector.x,
		monsterTransform->GetWorldPosition().y + createVector.y,
		monsterTransform->GetWorldPosition().z + createVector.z });

	// ����
	monster->GetScene()->AddGameObject(object);
}
