#include "CapsuleCollider.h"

fq::game_module::CapsuleCollider::CapsuleCollider()
{

}

fq::game_module::CapsuleCollider::~CapsuleCollider()
{

}

fq::game_module::Component* fq::game_module::CapsuleCollider::Clone(Component* clone /* = nullptr */) const
{
	CapsuleCollider* cloneCollider = static_cast<CapsuleCollider*>(clone);

	if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCollider = new CapsuleCollider(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCollider = *this;
	}

	return cloneCollider;
}

entt::meta_handle fq::game_module::CapsuleCollider::GetHandle()
{
	return *this;
}

float fq::game_module::CapsuleCollider::GetRadius() const
{
	return mCapsuleInfomation.raidus;
}

void fq::game_module::CapsuleCollider::SetRadius(float radius)
{
	mCapsuleInfomation.raidus = radius;
}
