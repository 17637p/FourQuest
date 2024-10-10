#include "TriangleCollider.h"


namespace fq::game_module
{
	TriangleCollider::TriangleCollider()
		: mType()
		, mOffset()
		, mTriangleMeshinfo()
		, mCollisionCount()
	{
	}

	TriangleCollider::~TriangleCollider()
	{
	}

	float TriangleCollider::GetStaticFriction() const
	{
		return mTriangleMeshinfo.colliderInfo.staticFriction;
	}

	void TriangleCollider::SetStaticFriction(float friction)
	{
		mTriangleMeshinfo.colliderInfo.staticFriction = friction;
	}

	float TriangleCollider::GetDynamicFriction() const
	{
		return mTriangleMeshinfo.colliderInfo.dynamicFriction;
	}

	void TriangleCollider::SetDynamicFriction(float friction)
	{
		mTriangleMeshinfo.colliderInfo.dynamicFriction = friction;
	}

	float TriangleCollider::GetRestituion() const
	{
		return mTriangleMeshinfo.colliderInfo.restitution;
	}

	void TriangleCollider::SetRestituion(float restitution)
	{
		mTriangleMeshinfo.colliderInfo.restitution = restitution;
	}

	float TriangleCollider::GetDensity() const
	{
		return mTriangleMeshinfo.colliderInfo.density;
	}

	void TriangleCollider::SetDensity(float density)
	{
		mTriangleMeshinfo.colliderInfo.density = density;
	}

	DirectX::SimpleMath::Vector3 TriangleCollider::GetOffset() const
	{
		return mOffset;
	}

	void TriangleCollider::SetOffset(DirectX::SimpleMath::Vector3 offset)
	{
		mOffset = offset;
	}

	entt::meta_handle TriangleCollider::GetHandle()
	{
		return *this;
	}

	std::shared_ptr<Component> TriangleCollider::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneCollider = std::dynamic_pointer_cast<TriangleCollider>(clone);

		if (cloneCollider == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneCollider = ObjectPool::GetInstance()->Assign<TriangleCollider>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneCollider = *this;
		}

		cloneCollider->mTriangleMeshinfo.colliderInfo.id = fq::physics::unregisterID;
		cloneCollider->mCollisionCount = 0;

		return cloneCollider;
	}
}

