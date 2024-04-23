#pragma once
#include "FQCommon.h"

namespace fq::physics
{
	enum class ECollisionType
	{
		TRIGGER,
		COLLISION
	};

	struct BoxCollisionInfo
	{
		fq::common::Transform collisionTransform;
		DirectX::SimpleMath::Vector3 boxExtent;
		float density;
	};
	struct SphereCollisionInfo
	{
		fq::common::Transform collisionTransform;
		float raidus;
		float density;
	};
	struct CapsuleCollisionInfo
	{
		fq::common::Transform collisionTransform;
		float raidus;
		float halfHeight;
		float density;
	};
	struct ConvexMeshCollisionInfo
	{
		fq::common::Transform collisionTransform;
		DirectX::SimpleMath::Vector3* vertices;
		int vertexSize;
		float density;
	};
}