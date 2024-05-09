#include "RigidBody.h"

fq::game_module::RigidBody::RigidBody()
	:mbIsStatic(false)
{
}

fq::game_module::RigidBody::~RigidBody()
{

}

fq::game_module::Component* fq::game_module::RigidBody::Clone(Component* clone /* = nullptr */) const
{
	RigidBody* cloneRigid = static_cast<RigidBody*>(clone);

	if (cloneRigid == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneRigid = new RigidBody(*this);
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
