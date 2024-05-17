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
		ELightType type;
		DirectX::SimpleMath::Color color = { 1.0f, 0.0f, 1.0f, 1.0f };
		float intensity = 1.0f;

		// Point, Spot
		DirectX::SimpleMath::Vector3 attenuation = { 0, 0, 1 };
		float range = 10;
		DirectX::SimpleMath::Vector3 position;

		// Spot
		float spot = 1; //Todo: �Ƹ��� spot �����ΰ� ������ Ȯ���غ��� �ٲٱ�

		// Directional, Spot
		DirectX::SimpleMath::Vector3 direction;
	};

	struct MeshObjectInfo
	{
		std::string ModelPath;
		std::string MeshName;
		std::vector<std::string> MaterialNames;
		DirectX::SimpleMath::Matrix Transform;
	};

	struct AnimationInfo
	{
		std::string ModelPath;
		std::string AnimationName; // Model Data�� Animation �̸�
		std::string AnimationKey; // ISkinnedMeshObject SetAnimation���� ����� �̸�
	};

	struct TerrainLayer
	{
		std::wstring BaseColor;
		std::wstring NormalMap;

		float Metalic;
		float Roughness;

		float TileSizeX;
		float TileSizeY;
		float TileOffsetX;
		float TileOffsetY;
	};

	struct TerrainMaterialInfo
	{
		//unsigned short NumOfTexture; // �ϴ� �ִ� 4

		//std::vector<std::wstring> BaseColorFileNames;
		//std::vector<std::wstring> MetalnessFileNames;
		//std::vector<std::wstring> RoughnessFileNames;
		//std::vector<std::wstring> NormalFileNames;

		std::vector<TerrainLayer> Layers; // �ִ� 4

		std::wstring AlPhaFileName; // R���� BaseColor1, G���� 2, B���� 3, A���� 4
	};
}

namespace fq::graphics::debug
{
	struct SphereInfo
	{
		DirectX::BoundingSphere Sphere;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct AABBInfo
	{
		DirectX::BoundingBox AABB;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct OBBInfo
	{
		DirectX::BoundingOrientedBox OBB;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct FrustumInfo
	{
		DirectX::BoundingFrustum Frustum;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct GridInfo
	{
		DirectX::SimpleMath::Vector3 Origin;
		DirectX::SimpleMath::Vector3 XAxis;
		DirectX::SimpleMath::Vector3 YAxis;
		size_t XDivision;
		size_t YDivision;
		float GridSize;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct RingInfo
	{
		DirectX::SimpleMath::Vector3 Origin;
		DirectX::SimpleMath::Vector3 MajorAxis;
		DirectX::SimpleMath::Vector3 MinorAxis;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct RayInfo
	{
		DirectX::SimpleMath::Vector3 Origin;
		DirectX::SimpleMath::Vector3 Direction;
		bool Normalize = true;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};

	struct PolygonInfo
	{
		std::vector<DirectX::SimpleMath::Vector3> Points;
		DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
	};
}