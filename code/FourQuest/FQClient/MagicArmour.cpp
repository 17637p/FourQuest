#include "MagicArmour.h"

#include "LinearAttack.h"
#include "Attack.h"
#include "Player.h"
#include "DamageCalculation.h"

fq::client::MagicArmour::MagicArmour()
	:mPlayer(nullptr)
	,mMagicBall{}
	,mAOE{}
	,mRazer{}
	,mAttackWarningUI{}
	,mMagicBallSpeed(10.f)
{}

fq::client::MagicArmour::~MagicArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MagicArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<MagicArmour>(clone);

	if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<MagicArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneArmour = *this;
	}

	cloneArmour->mPlayer = nullptr;

	return cloneArmour;
}

void fq::client::MagicArmour::EmitMagicBall()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMagicBall);
	auto& attackObj = *(instance.begin());

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	// 공격 위치 설정
	DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
	pos.y += 1.f;
	attackT->SetLocalPosition(pos);

	// 매직볼 공격력 계산 
	float attackPower = mPlayer->GetAttackPower();
	attackComponent->SetAttackPower(dc::GetMagicBallDamage(attackPower));

	// 공격 방향 설정
	auto linearAttack = attackObj->GetComponent<LinearAttack>();

	auto direction = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Forward();
	direction.Normalize();
	linearAttack->SetMoveSpeed(mMagicBallSpeed);
	linearAttack->SetMoveDirection(direction);

	GetScene()->AddGameObject(attackObj);
}

void fq::client::MagicArmour::EmitAOE(DirectX::SimpleMath::Vector3 attackPoint)
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mAOE);
	auto& attackObj = *(instance.begin());

	// 공격 설정
	auto attackComponent = attackObj->GetComponent<client::Attack>();
	attackComponent->SetAttacker(GetGameObject());
	auto attackT = attackObj->GetComponent<game_module::Transform>();

	// 공격 위치 설정
	attackT->SetLocalPosition(attackPoint);

	// AOE 공격력 계산 

}

void fq::client::MagicArmour::EmitRazer()
{

}

void fq::client::MagicArmour::OnStart()
{
	mTransform = GetComponent<game_module::Transform>();
	mPlayer = GetComponent<Player>();
}

void fq::client::MagicArmour::OnUpdate(float dt)
{
}
