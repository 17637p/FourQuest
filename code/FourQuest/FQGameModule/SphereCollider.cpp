#include "SphereCollider.h"

fq::game_module::SphereCollider::~SphereCollider()
{}

fq::game_module::SphereCollider::SphereCollider()
	:mCollisionCount(0)
	,mOffset{}
{
	mSphereColliderInfo.raidus = 1.f;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SphereCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCollider = std::dynamic_pointer_cast<SphereCollider>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = ObjectPool::GetInstance()->Assign<SphereCollider>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	cloneCollider->mSphereColliderInfo.colliderInfo.id = fq::physics::unregisterID;

	return cloneCollider;
}

entt::meta_handle fq::game_module::SphereCollider::GetHandle()
{
	return *this;
}

float fq::game_module::SphereCollider::GetStaticFriction() const
{
	return mSphereColliderInfo.colliderInfo.staticFriction;
}

void fq::game_module::SphereCollider::SetStaticFriction(float friction)
{
	mSphereColliderInfo.colliderInfo.staticFriction = friction;
}

float fq::game_module::SphereCollider::GetDynamicFriction() const
{
	return mSphereColliderInfo.colliderInfo.dynamicFriction;
}

void fq::game_module::SphereCollider::SetDynamicFriction(float friction)
{
	mSphereColliderInfo.colliderInfo.dynamicFriction = friction;
}

float fq::game_module::SphereCollider::GetRestituion() const
{
	return mSphereColliderInfo.colliderInfo.restitution;
}

void fq::game_module::SphereCollider::SetRestituion(float restitution)
{
	mSphereColliderInfo.colliderInfo.restitution = restitution;
}

float fq::game_module::SphereCollider::GetDensity() const
{
	return mSphereColliderInfo.colliderInfo.density;
}

void fq::game_module::SphereCollider::SetDensity(float density)
{
	mSphereColliderInfo.colliderInfo.density = density;
}

void fq::game_module::SphereCollider::SetRadius(float radius)
{
	mSphereColliderInfo.raidus = max(0.00001f, radius);
}

void fq::game_module::SphereCollider::OnCollisionEnter(const Collision& collision)
{
	++mCollisionCount;

}

void fq::game_module::SphereCollider::OnCollisionExit(const Collision& collision)
{
	//assert(mCollisionCount != 0);
	--mCollisionCount;
}

void fq::game_module::SphereCollider::OnTriggerEnter(const Collision& collision)
{
	++mCollisionCount;

}

void fq::game_module::SphereCollider::OnTriggerExit(const Collision& collision)
{
	//assert(mCollisionCount != 0);
	--mCollisionCount;
}
