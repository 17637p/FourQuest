#pragma once

#include <directxtk/SimpleMath.h>
#include <string>

namespace fq::graphics
{
	struct CameraInfo
	{
		bool isPerspective = false;
		float filedOfView = 0;
		float nearPlain = 0;
		float farPlain = 0;
	};

	enum class LightType
	{
		Directional,
		Point,
		Spot
	};

	struct LightInfo
	{
		// Common
		unsigned int id = -1;

		LightType type;
		DirectX::SimpleMath::Vector3 color = { -1.0f, -1.0f, -1.0f };
		float intensity = -1.0f;

		// Point, Spot
		DirectX::SimpleMath::Vector3 attenuation = { 0, 0, 1 };
		float range = 10;

		// Spot
		float spot = 1; //Todo: 아마도 spot 지름인거 같은데 확인해보고 바꾸기
	};

	struct MeshObjectInfo
	{
		std::string MeshKey;
		std::vector<std::string> MaterialKeys;
		DirectX::SimpleMath::Matrix Transform;
	};
}

