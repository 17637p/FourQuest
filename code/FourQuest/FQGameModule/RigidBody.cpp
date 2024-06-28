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

	if (cloneRigid == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneRigid = ObjectPool::GetInstance()->Assign<RigidBody>(*this);
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

void fq::game_module::RigidBody::AddLinearVelocity(Vector3 velocity)
{
	mLinearVelocity += velocity;
}
