#include "MeleeMonsterExplosion.h"

#include "../FQGameModule/Transform.h"
#include "Attack.h"
#include "KnockBack.h"

fq::client::MeleeMonsterExplosion::MeleeMonsterExplosion()
	:mExplosionRadius(1.f)
	,mWarningUI{}
	,mExplosion{}
	,mExplosionDamage{0.f}
	,mExplosionTime{3.f}
{}

fq::client::MeleeMonsterExplosion::~MeleeMonsterExplosion()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::MeleeMonsterExplosion::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<MeleeMonsterExplosion> cloneMonster = std::dynamic_pointer_cast<MeleeMonsterExplosion>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterExplosion>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::MeleeMonsterExplosion::OnStart()
{
	auto animator = GetComponent<game_module::Animator>();
	animator->SetParameterBoolean("OnExplosion", true);
}

void fq::client::MeleeMonsterExplosion::Explode()
{
	using namespace game_module;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mExplosion);
	auto& attackObj = *(instance.begin());
	auto attackT = attackObj->GetComponent<Transform>();
	auto monsterT = GetComponent<Transform>();
	
	// 폭발 트랜스폼 설정
	auto attackPos = monsterT->GetWorldPosition();
	DirectX::SimpleMath::Vector3 scale = { mExplosionRadius, mExplosionRadius, mExplosionRadius };
	attackT->GenerateWorld(attackPos, attackT->GetWorldRotation(), scale);

	// 폭발 공격 설정
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<client::Attack>();

	attackInfo.damage = mExplosionDamage;
	attackInfo.type = EKnockBackType::TargetPosition;

	attackComponent->Set(attackInfo);
	GetScene()->AddGameObject(attackObj);

	// 자식에는 몬스터를 공격하는 콜라이더가 있어서 함께 설정 
	auto& child = *(instance.begin()+1);
	child->GetComponent<Attack>()->Set(attackInfo);

	// 자기 자신을 사라집니다
	GetScene()->DestroyGameObject(GetGameObject());

	// 폭발 소리 
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Explode_Boom", false ,  fq::sound::EChannel::SE });
}

std::shared_ptr<fq::game_module::GameObject> fq::client::MeleeMonsterExplosion::EmitExplosionEffect()
{
	// 넉백 삭제	
	GetGameObject()->RemoveComponent<KnockBack>();

	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mWarningUI);
	auto& warningObj = *(instance.begin());
	auto warningT = warningObj->GetComponent<Transform>();
	auto monsterT = GetComponent<Transform>();

	// 경고UI 트랜폼 설정
	auto position = monsterT->GetWorldPosition();
	DirectX::SimpleMath::Vector3 scale = { mExplosionRadius, mExplosionRadius, mExplosionRadius };
	warningT->GenerateWorld(position, warningT->GetWorldRotation(), scale);

	GetScene()->AddGameObject(warningObj);

	return warningObj;
}
