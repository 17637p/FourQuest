#pragma once
#include <vector>

#include <directxtk/SimpleMath.h>

namespace fq
{
	struct Transform
	{
		DirectX::SimpleMath::Vector3 localPosition;
		DirectX::SimpleMath::Quaternion localrotation;
		DirectX::SimpleMath::Vector3 localscale;

		DirectX::SimpleMath::Vector3 worldPosition;
		DirectX::SimpleMath::Quaternion worldlocalrotation;
		DirectX::SimpleMath::Vector3 worldlocalscale;

		DirectX::SimpleMath::Matrix localMatrix;
		DirectX::SimpleMath::Matrix worldMatrix;

		Transform* parent;
	};
}

