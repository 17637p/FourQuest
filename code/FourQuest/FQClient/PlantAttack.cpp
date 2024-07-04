#include "PlantAttack.h"

fq::client::PlantAttack::PlantAttack()
	:mMoveDirection{ DirectX::SimpleMath::Vector3::Up }
	, mMoveSpeed(1.f)
{

}

fq::client::PlantAttack::~PlantAttack()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::PlantAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<PlantAttack> cloneMonster = std::dynamic_pointer_cast<PlantAttack>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<PlantAttack>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::PlantAttack::OnUpdate(float dt)
{
	auto transform = GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 deltaPosition = mMoveDirection * dt * mMoveSpeed;
	transform->AddPosition(deltaPosition);
}
