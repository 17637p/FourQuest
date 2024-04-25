#include "SphereCollider.h"

fq::game_module::SphereCollider::~SphereCollider()
{}

fq::game_module::SphereCollider::SphereCollider()
{

}

fq::game_module::Component* fq::game_module::SphereCollider::Clone(Component* clone /* = nullptr */) const
{
	SphereCollider* cloneCollider = static_cast<SphereCollider*>(clone);

	if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCollider = new SphereCollider(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCollider = *this;
	}

	return cloneCollider;
}

entt::meta_handle fq::game_module::SphereCollider::GetHandle()
{
	return *this;
}
