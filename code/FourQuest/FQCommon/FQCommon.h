#pragma once


#include <vector>
#include <directxtk/SimpleMath.h>

namespace fq::common
{
	struct Transform
	{
		DirectX::SimpleMath::Vector3	localPosition;
		DirectX::SimpleMath::Quaternion localRotation;
		DirectX::SimpleMath::Vector3	localScale;

		DirectX::SimpleMath::Vector3	worldPosition;
		DirectX::SimpleMath::Quaternion worldRotation;
		DirectX::SimpleMath::Vector3	worldScale;

		DirectX::SimpleMath::Matrix localMatrix;
		DirectX::SimpleMath::Matrix worldMatrix;
		//Transform* parent;
	};
}




