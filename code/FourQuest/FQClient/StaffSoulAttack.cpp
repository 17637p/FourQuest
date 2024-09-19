#include "StaffSoulAttack.h"

#include "Attack.h"
#include "Player.h"
#include "DamageCalculation.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"

fq::client::StaffSoulAttack::StaffSoulAttack()
	:mPlayer(nullptr)
	, mAttackEmitTick(1.f)
	, mAttack{}
	, mAttackEmitElapsedTime(0.f)
	, mAttackDuration(5.f)
	, mAttackElapsedTime(0.f)
{}

fq::client::StaffSoulAttack::~StaffSoulAttack()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::StaffSoulAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<StaffSoulAttack> cloneAttack = std::dynamic_pointer_cast<StaffSoulAttack>(clone);

	if (cloneAttack == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAttack = game_module::ObjectPool::GetInstance()->Assign<StaffSoulAttack>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAttack = *this;
	}

	return cloneAttack;
}

void fq::client::StaffSoulAttack::OnUpdate(float dt)
{
	if (mPlayerObject->IsDestroyed())
	{
		GetScene()->DestroyGameObject(GetGameObject());
		return;
	}

	mAttackEmitElapsedTime += dt;

	if (mAttackEmitElapsedTime >= mAttackEmitTick)
	{
		mAttackEmitElapsedTime -= mAttackEmitTick;
		emitAttack();
	}

	mAttackElapsedTime += dt;

	if (mAttackDuration <= mAttackElapsedTime)
	{
		GetScene()->DestroyGameObject(GetGameObject());
	}
}

void fq::client::StaffSoulAttack::emitAttack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAttack);
	auto& attackObj = *(instance.begin());
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	auto transform = GetTransform();
	attackT->SetWorldPosition(transform->GetWorldPosition());

	// ���� ����
	AttackInfo attackInfo{};
	attackInfo.attacker = mPlayer->GetGameObject();
	attackInfo.damage = dc::GetStaffSoulDamage(mPlayer->GetAttackPower());
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::StaffSoulAttack::SetPlayer(fq::client::Player* player)
{
	mPlayer = player;
	mPlayerObject = player->GetGameObject()->shared_from_this();
}
