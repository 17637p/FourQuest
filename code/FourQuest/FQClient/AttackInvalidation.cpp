#include "AttackInvalidation.h"

fq::client::AttackInvalidation::AttackInvalidation()
{

}

fq::client::AttackInvalidation::~AttackInvalidation()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::AttackInvalidation::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<AttackInvalidation> cloneInvalidation = std::dynamic_pointer_cast<AttackInvalidation>(clone);

	if (cloneInvalidation == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneInvalidation = game_module::ObjectPool::GetInstance()->Assign<AttackInvalidation>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneInvalidation = *this;
	}

	return cloneInvalidation;
}

void fq::client::AttackInvalidation::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::MonsterAttack
		&& collision.other->GetName().find("Plant") != std::string::npos)
	{
		GetScene()->DestroyGameObject(collision.other);
	}
}
