#include "BoxCollider.h"

fq::game_module::BoxCollider::BoxCollider()
	:mBoxinfomation{}
	,mType(fq::physics::EColliderType::COLLISION)
{
	mBoxinfomation.boxExtent = { 1.f, 1.f, 1.f };
}

fq::game_module::BoxCollider::~BoxCollider()
{

}

void fq::game_module::BoxCollider::SetExtent(DirectX::SimpleMath::Vector3 extent)
{
	mBoxinfomation.boxExtent.x = max(extent.x, 0.000001f);
	mBoxinfomation.boxExtent.y = max(extent.y, 0.000001f);
	mBoxinfomation.boxExtent.z = max(extent.z, 0.000001f);
}

DirectX::SimpleMath::Vector3 fq::game_module::BoxCollider::GetExtent() const
{
	return mBoxinfomation.boxExtent;
}

entt::meta_handle fq::game_module::BoxCollider::GetHandle()
{
	return *this;
}

fq::game_module::Component* fq::game_module::BoxCollider::Clone(Component* clone /* = nullptr */) const
{
	BoxCollider* cloneCollider = static_cast<BoxCollider*>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = new BoxCollider(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

	cloneCollider->mBoxinfomation.colliderInfo.id = fq::physics::unregisterID;

	return cloneCollider;
}

float fq::game_module::BoxCollider::GetStaticFriction() const
{
	return mBoxinfomation.colliderInfo.staticFriction;
}

void fq::game_module::BoxCollider::SetStaticFriction(float friction)
{
	mBoxinfomation.colliderInfo.staticFriction = friction;
}

float fq::game_module::BoxCollider::GetDynamicFriction() const
{
	return mBoxinfomation.colliderInfo.dynamicFriction;
}

void fq::game_module::BoxCollider::SetDynamicFriction(float friction)
{
	mBoxinfomation.colliderInfo.dynamicFriction = friction;
}

float fq::game_module::BoxCollider::GetRestituion() const
{
	return mBoxinfomation.colliderInfo.restitution;
}

void fq::game_module::BoxCollider::SetRestituion(float restitution)
{
	mBoxinfomation.colliderInfo.restitution = restitution;
}

float fq::game_module::BoxCollider::GetDensity() const
{
	return mBoxinfomation.colliderInfo.density;
}

void fq::game_module::BoxCollider::SetDensity(float density)
{
	mBoxinfomation.colliderInfo.density = density;
}

