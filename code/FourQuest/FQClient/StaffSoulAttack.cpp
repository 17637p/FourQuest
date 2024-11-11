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
	, mKnockBackPower(5.)
{}

fq::client::StaffSoulAttack::~StaffSoulAttack()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::StaffSoulAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<StaffSoulAttack> cloneAttack = std::dynamic_pointer_cast<StaffSoulAttack>(clone);

	if (cloneAttack == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAttack = game_module::ObjectPool::GetInstance()->Assign<StaffSoulAttack>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAttack = *this;
	}

	return cloneAttack;
}

void fq::client::StaffSoulAttack::OnUpdate(float dt)
{
	// 프리팹 임시로 띄워보고 싶을 때 터져서 예외처리 추가_홍지환
	if (mPlayerObject == nullptr)
	{
		return;
	}

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

	// 공격 설정
	AttackInfo attackInfo{};
	attackInfo.attacker = mPlayer->GetGameObject();
	attackInfo.damage = dc::GetStaffSoulDamage(mPlayer->GetAttackPower());
	attackInfo.knocBackPower = mKnockBackPower;
	attackInfo.attackPosition = transform->GetWorldPosition();
	attackInfo.type = EKnockBackType::TargetPosition;
	attackInfo.attackType = AttackInfo::EAttackType::MagicSoul;
	attackComponent->Set(attackInfo);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::StaffSoulAttack::SetPlayer(fq::client::Player* player)
{
	mPlayer = player;
	mPlayerObject = player->GetGameObject()->shared_from_this();
}
