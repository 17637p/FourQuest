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

	if (cloneRigid == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneRigid = new RigidBody(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneRigid = *this;
	}

	return cloneRigid;
}

entt::meta_handle fq::game_module::RigidBody::GetHandle()
{
	return *this;
}
