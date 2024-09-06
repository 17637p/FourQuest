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

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<MeleeMonsterExplosion>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
	
	// ���� Ʈ������ ����
	auto attackPos = monsterT->GetWorldPosition();
	DirectX::SimpleMath::Vector3 scale = { mExplosionRadius, mExplosionRadius, mExplosionRadius };
	attackT->GenerateWorld(attackPos, attackT->GetWorldRotation(), scale);

	// ���� ���� ����
	AttackInfo attackInfo{};
	auto attackComponent = attackObj->GetComponent<client::Attack>();

	attackInfo.damage = mExplosionDamage;
	attackInfo.type = EKnockBackType::TargetPosition;

	attackComponent->Set(attackInfo);
	GetScene()->AddGameObject(attackObj);

	// �ڽĿ��� ���͸� �����ϴ� �ݶ��̴��� �־ �Բ� ���� 
	auto& child = *(instance.begin()+1);
	child->GetComponent<Attack>()->Set(attackInfo);

	// �ڱ� �ڽ��� ������ϴ�
	GetScene()->DestroyGameObject(GetGameObject());

	// ���� �Ҹ� 
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "MM_Explode_Boom", false ,  fq::sound::EChannel::SE });
}

std::shared_ptr<fq::game_module::GameObject> fq::client::MeleeMonsterExplosion::EmitExplosionEffect()
{
	// �˹� ����	
	GetGameObject()->RemoveComponent<KnockBack>();

	using namespace game_module;
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mWarningUI);
	auto& warningObj = *(instance.begin());
	auto warningT = warningObj->GetComponent<Transform>();
	auto monsterT = GetComponent<Transform>();

	// ���UI Ʈ���� ����
	auto position = monsterT->GetWorldPosition();
	DirectX::SimpleMath::Vector3 scale = { mExplosionRadius, mExplosionRadius, mExplosionRadius };
	warningT->GenerateWorld(position, warningT->GetWorldRotation(), scale);

	GetScene()->AddGameObject(warningObj);

	return warningObj;
}
