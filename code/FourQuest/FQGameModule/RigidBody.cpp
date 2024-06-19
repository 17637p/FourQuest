#include "RigidBody.h"

fq::game_module::RigidBody::RigidBody()
	:mBodyType(EBodyType::Dynamic)
{
}

fq::game_module::RigidBody::~RigidBody()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::RigidBody::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneRigid = std::dynamic_pointer_cast<RigidBody>(clone);

	if (cloneRigid == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneRigid = ObjectPool::GetInstance()->Assign<RigidBody>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneRigid = *this;
	}

	return cloneRigid;
}

entt::meta_handle fq::game_module::RigidBody::GetHandle()
{
	return *this;
}

void fq::game_module::RigidBody::AddLinearVelocity(Vector3 velocity)
{
	mLinearVelocity += velocity;
}
