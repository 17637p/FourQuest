#include "CapsuleCollider.h"

const DirectX::SimpleMath::Quaternion fq::game_module::CapsuleCollider::YtoXRoation
= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Forward, -DirectX::XM_PIDIV2);

const DirectX::SimpleMath::Quaternion fq::game_module::CapsuleCollider::XtoYRoation
= DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(DirectX::SimpleMath::Vector3::Forward, DirectX::XM_PIDIV2);

fq::game_module::CapsuleCollider::CapsuleCollider()
	:mCapsuleInfomation{}
	, mCollisionCount(0)
	, mDirection(EDirection::YAxis)
{
	mCapsuleInfomation.halfHeight = 1.f;
	mCapsuleInfomation.raidus = 1.f;
}

fq::game_module::CapsuleCollider::~CapsuleCollider()
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::CapsuleCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCollider = std::dynamic_pointer_cast<CapsuleCollider>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = ObjectPool::GetInstance()->Assign<CapsuleCollider>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	cloneCollider->mCapsuleInfomation.colliderInfo.id = fq::physics::unregisterID;

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
	mCapsuleInfomation.raidus = max(0.0001f, radius);
}

void fq::game_module::CapsuleCollider::SetHalfHegiht(float halfHegiht)
{
	mCapsuleInfomation.halfHeight = halfHegiht;
}

void fq::game_module::CapsuleCollider::SetDensity(float density)
{
	mCapsuleInfomation.colliderInfo.density = density;
}

float fq::game_module::CapsuleCollider::GetDensity() const
{
	return mCapsuleInfomation.colliderInfo.density;
}

void fq::game_module::CapsuleCollider::SetRestituion(float restitution)
{
	mCapsuleInfomation.colliderInfo.restitution = restitution;
}

float fq::game_module::CapsuleCollider::GetRestituion() const
{
	return mCapsuleInfomation.colliderInfo.restitution;
}

void fq::game_module::CapsuleCollider::SetDynamicFriction(float friction)
{
	mCapsuleInfomation.colliderInfo.dynamicFriction = friction;
}

float fq::game_module::CapsuleCollider::GetDynamicFriction() const
{
	return mCapsuleInfomation.colliderInfo.dynamicFriction;
}

void fq::game_module::CapsuleCollider::SetStaticFriction(float friction)
{
	mCapsuleInfomation.colliderInfo.staticFriction = friction;
}

float fq::game_module::CapsuleCollider::GetStaticFriction() const
{
	return mCapsuleInfomation.colliderInfo.staticFriction;
}

void fq::game_module::CapsuleCollider::OnCollisionEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CapsuleCollider::OnCollisionExit(const Collision& collision)
{
	assert(mCollisionCount != 0);
	--mCollisionCount;
}

void fq::game_module::CapsuleCollider::OnTriggerEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::CapsuleCollider::OnTriggerExit(const Collision& collision)
{
	assert(mCollisionCount != 0);
	--mCollisionCount;
}

