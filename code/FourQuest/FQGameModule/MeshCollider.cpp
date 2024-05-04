#include "MeshCollider.h"

#include <algorithm>
fq::game_module::MeshCollider::MeshCollider()
	:mCollisionCount(0)
{}

fq::game_module::MeshCollider::~MeshCollider()
{

}

fq::game_module::Component* fq::game_module::MeshCollider::Clone(Component* clone /* = nullptr */) const
{
	MeshCollider* cloneCollider = static_cast<MeshCollider*>(clone);

	if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCollider = new MeshCollider(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCollider = *this;
	}

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

unsigned char fq::game_module::MeshCollider::GetConvexVertexLimit() const
{
	return mConvexMeshInfomation.convexVertexLimit;
}

void fq::game_module::MeshCollider::SetConvexPolygonLimit(unsigned char limit)
{
	mConvexMeshInfomation.convexPolygonLimit = 
		std::clamp(limit, (unsigned char)4, (unsigned char)255);
}

void fq::game_module::MeshCollider::SetConvexVertexLimit(unsigned char limit)
{
	mConvexMeshInfomation.convexVertexLimit =
		std::clamp(limit, (unsigned char)4, (unsigned char)255);
}
