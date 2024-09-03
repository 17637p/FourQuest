#include "ParabolaAttack.h"

#include "../FQGameModule/Transform.h"

std::shared_ptr<fq::game_module::Component> fq::client::ParabolaAttack::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<ParabolaAttack> cloneMonster = std::dynamic_pointer_cast<ParabolaAttack>(clone);

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<ParabolaAttack>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

fq::client::ParabolaAttack::~ParabolaAttack()
{}

fq::client::ParabolaAttack::ParabolaAttack()
	:mVelocity{}
	,mMoveDistance(1.f)
{}

void fq::client::ParabolaAttack::OnFixedUpdate(float dt)
{
	auto transform = GetComponent<game_module::Transform>();
	constexpr float Gravity = -0.5f;
	mVelocity.y += Gravity * dt;
	auto deltaPosition = mVelocity * dt;

	auto position = transform->GetWorldPosition();
	transform->SetWorldPosition(position + deltaPosition);
}

void fq::client::ParabolaAttack::OnStart()
{
}
