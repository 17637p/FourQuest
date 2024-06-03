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

	struct UIInfo
	{
		float StartX; // �»�� �� ��ġ 
		float StartY;
		float Width;
		float Height;

		float Alpha; // 1�� ������ 0�� ���� 

		unsigned int Layer; // ���� ���� ���� Ŭ ���� �Ʒ��� ���

		float XRatio; // 1�� ���δ� ���
		float YRatio; // 1�� ���δ� ���

		std::string ImagePath;

		// ���� ����
		float RotationAngle;
		float ScaleX;
		float ScaleY;
	};

	struct TerrainLayer
	{
		std::string BaseColor;
		std::string NormalMap;

		float Metalic = 0.f;
		float Roughness = 0.f;

		float TileSizeX = 1.f;
		float TileSizeY = 1.f;
		float TileOffsetX = 0.f;
		float TileOffsetY = 0.f;
	};

	struct TerrainMaterialInfo
	{
		std::vector<TerrainLayer> Layers; // �ִ� 4
		std::string AlPhaFileName; // R���� BaseColor1, G���� 2, B���� 3, A���� 4

		std::string HeightFileName;
		float HeightScale; // ��ü ���� (Length)
		float Width; // ���� ũ��
		float Height; // ���� ũ��
	};

	struct ParticleSystemInfo
	{
		// main module
		enum { MAX_PARTICLE_COUNT = 1000 };

		enum class EOption
		{
			Constant,
			RandomBetweenTwoConstant,
		};

		float Duration = 5; // �� ����ð�
		bool bIsLooping = true;

		EOption StartDelayOption = EOption::Constant;
		float StartDelay[2] = { 0.f, }; // ���� ���� �ð�

		EOption StartLifeTimeOption = EOption::Constant;
		float StartLifeTime[2] = { 5.f, }; // ��ƼŬ ���� �ð�

		EOption StartSpeedOption = EOption::Constant;
		float StartSpeed[2] = { 5.f, }; // ���� �ӵ�

		EOption StartSizeOption = EOption::Constant;
		DirectX::SimpleMath::Vector2 StartSize[2] = { {1.f, 1.f} , }; // ��ƼŬ ũ��

		EOption StartRotationOption = EOption::Constant;
		float StartRotation[2] = { 0.f, }; // ��ƼŬ ȸ��

		EOption StartColorOption = EOption::Constant;
		DirectX::SimpleMath::Color StartColor[2] = { { 1.f, 1.f, 1.f, 1.f }, }; // ��ƼŬ ����

		EOption GravityModifierOption = EOption::Constant;
		float GravityModifier[2] = { 0.f, }; // ��ƼŬ �߷�

		float SimulationSpeed = 1.f; // �ùķ��̼� ����ġ

		size_t MaxParticleCount = MAX_PARTICLE_COUNT;

		int RandomSeed = rand(); // ��ƼŬ ������ ����� ���� �õ尪

		DirectX::SimpleMath::Vector3 WorldPosition = { 0, 0, 0 };
		DirectX::SimpleMath::Vector3 WorldRotation = { 0, 0, 0 };
		DirectX::SimpleMath::Vector3 WorldScale = { 1, 1, 1 };

		// emission module
		size_t RateOverTime = 10u; // �ð� ������ ����Ǵ� ��ƼŬ ��
		size_t RateOverDistance = 0u; // �̵��� �Ÿ� ������ ����Ǵ� ��ƼŬ ��

		struct Burst
		{
			float TimePos = 0.f; // ó���� �ð�
			size_t Count = 30u; // ����� ��ƼŬ ��
			size_t Cycles = 1u; // ����Ʈ �ݺ� Ƚ��
			float Interval = 0.01f; // ����Ʈ ���� ����
			float Probability = 1.f; // 0 ~ 1 ������ Ȯ�� ���� ���
		};

		std::vector<Burst> Bursts;

		// shape module
		enum class EShape
		{
			Sphere,
			// Hemisphere,
			// Cone,
			// Donut,
			// Box,
			// Circle,
			// Rectangle,
			// Edge
		};

		enum class EMode
		{
			Random,
			// Loop,
			// PingPong,
			// BurstSpread
		};

		EShape ShapeType = EShape::Sphere; // ���� ���
		EMode ModeType = EMode::Random; // ���� ���
		float Angle = 25; // 
		float Radius = 1; // ������
		float DountRadius = 0.2f; // ���� ������
		float Arc = 360.f; // ȣ ũ��, ���� ���

		DirectX::SimpleMath::Vector3 Position = { 0, 0, 0 };
		DirectX::SimpleMath::Vector3 Rotation = { 0, 0, 0 };
		DirectX::SimpleMath::Vector3 Scale = { 1, 1, 1 };

		float Speed = 1.f;	 // �� ���ʹ� ���� �̹ݿ�
		float Spread = 0.f;	 // �� ���ʹ� ���� �̹ݿ�
		float RadiusThickness = 0.f; // �Թ�ü�� ��� �κ� ���� ����� ������ 0 ~ 1f

		// render module
		enum class ERenderMode
		{
			Billboard,
		};
		enum class EBlendMode
		{
			Additive,
			//Subtractive,
			//Moudulate
		};

		ERenderMode RenderMode = ERenderMode::Billboard;
		EBlendMode BlendMode = EBlendMode::Additive;
		std::wstring TexturePath = L"";
	};

	namespace debug
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
	};
};