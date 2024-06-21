#include "MeshCollider.h"

#include <algorithm>
fq::game_module::MeshCollider::MeshCollider()
	:mCollisionCount(0)
{}

fq::game_module::MeshCollider::~MeshCollider()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::MeshCollider::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCollider = std::dynamic_pointer_cast<MeshCollider>(clone);

	if (cloneCollider == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCollider = ObjectPool::GetInstance()->Assign<MeshCollider>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCollider = *this;
	}

	cloneCollider->mConvexMeshInfomation.colliderInfo.id = fq::physics::unregisterID;
	cloneCollider->mCollisionCount = 0;

	return cloneCollider;
}

entt::meta_handle fq::game_module::MeshCollider::GetHandle()
{
	return *this;
}

void fq::game_module::MeshCollider::OnTriggerExit(const Collision& collision)
{
	--mCollisionCount;
}

void fq::game_module::MeshCollider::OnTriggerEnter(const Collision& collision)
{
	++mCollisionCount;
}

void fq::game_module::MeshCollider::OnCollisionExit(const Collision& collision)
{
	--mCollisionCount;
}

void fq::game_module::MeshCollider::OnCollisionEnter(const Collision& collision)
{
	++mCollisionCount;
}

float fq::game_module::MeshCollider::GetStaticFriction() const
{
	return mConvexMeshInfomation.colliderInfo.staticFriction;
}

void fq::game_module::MeshCollider::SetStaticFriction(float friction)
{
	mConvexMeshInfomation.colliderInfo.staticFriction = friction;
}

float fq::game_module::MeshCollider::GetDynamicFriction() const
{
	return mConvexMeshInfomation.colliderInfo.dynamicFriction;
}

void fq::game_module::MeshCollider::SetDynamicFriction(float friction)
{
	mConvexMeshInfomation.colliderInfo.dynamicFriction = friction;
}

float fq::game_module::MeshCollider::GetRestituion() const
{
	return mConvexMeshInfomation.colliderInfo.restitution;
}

void fq::game_module::MeshCollider::SetRestituion(float restitution)
{
	mConvexMeshInfomation.colliderInfo.restitution = restitution;
}

float fq::game_module::MeshCollider::GetDensity() const
{
	return mConvexMeshInfomation.colliderInfo.density;
}

void fq::game_module::MeshCollider::SetDensity(float density)
{
	mConvexMeshInfomation.colliderInfo.density = density;
}

unsigned char fq::game_module::MeshCollider::GetConvexPolygonLimit() const
{
	return mConvexMeshInfomation.convexPolygonLimit;
}

void fq::game_module::MeshCollider::SetConvexPolygonLimit(unsigned char limit)
{
	mConvexMeshInfomation.convexPolygonLimit = 
		std::clamp(limit, (unsigned char)4, (unsigned char)255);
}
