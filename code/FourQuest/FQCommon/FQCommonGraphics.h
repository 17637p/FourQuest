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

	enum class ELightType
	{
		Directional,
		Point, 
		Spot
	};

	struct LightInfo
	{
		// Common
		unsigned int id = -1;

		ELightType type;
		DirectX::SimpleMath::Vector3 color = { -1.0f, -1.0f, -1.0f };
		float intensity = -1.0f;

		// Point, Spot
		DirectX::SimpleMath::Vector3 attenuation = { 0, 0, 1 };
		float range = 10;

		// Spot
		float spot = 1; //Todo: �Ƹ��� spot �����ΰ� ������ Ȯ���غ��� �ٲٱ�
	};

	struct MeshObjectInfo
	{
		std::string ModelPath;
		std::string MeshName;
		std::vector<std::string> MaterialNames;
		DirectX::SimpleMath::Matrix Transform;
	};
}
