#pragma once

#include <filesystem>
#include <string>
#include <directxtk/SimpleMath.h>
#include "../FQCommon/FQCommonLoader.h"

namespace fq::graphics
{
	struct CameraInfo
	{
		bool isPerspective = false;
		float fieldOfView = 0;
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
		float StartX = 0.f; // �»�� �� ��ġ 
		float StartY = 0.f;
		float Width = 100.f;
		float Height = 100.f;

		float Alpha = 1.f; // 1�� ������ 0�� ���� 

		unsigned int Layer = 0; // ���� ���� ���� Ŭ ���� �Ʒ��� ���

		float XRatio= 1.f; // 1�� ���δ� ���
		float YRatio= 1.f; // 1�� ���δ� ���

		std::string ImagePath;

		// ���� ����
		float RotationAngle = 0.f;
		float ScaleX = 1.f;
		float ScaleY = 1.f;
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

		std::string HeightFileName; // Raw ����
		float TextureWidth;
		float TextureHeight;

		float HeightScale; // ��ü ���� (Length)
		float TerrainWidth; // ���� ũ��
		float TerrainHeight; // ���� ũ��
	};

	struct ParticleInfo
	{
		enum { MAX_PARTICLE_COUNT = 1024 };

		enum class EOption
		{
			Constant,
			RandomBetweenTwoConstant,
		};

		struct Main
		{
			float Duration = 5; // �� ����ð�
			bool bIsLooping = true;

			EOption StartDelayOption = EOption::Constant;
			DirectX::SimpleMath::Vector2 StartDelay{ 0.f, 0.f }; // ���� ���� �ð�

			EOption StartLifeTimeOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartLifeTime{ 5.f, 5.f }; // ��ƼŬ ���� �ð�

			EOption StartSpeedOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartSpeed{ 5.f, 5.f }; // ���� �ӵ�

			EOption StartSizeOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartSize{ 1.f, 1.f }; // ��ƼŬ ũ��

			EOption StartRotationOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartRotation{ 0.f, 0.f }; // ��ƼŬ ȸ��

			EOption StartColorOption = EOption::Constant;
			DirectX::SimpleMath::Color StartColor0{ 1.f, 1.f, 1.f, 1.f }; // ��ƼŬ ����
			DirectX::SimpleMath::Color StartColor1{ 1.f, 1.f, 1.f, 1.f }; // ��ƼŬ ����

			EOption GravityModifierOption = EOption::Constant;
			DirectX::SimpleMath::Vector2 GravityModifier{ 0.f, 0.f }; // ��ƼŬ �߷�

			float SimulationSpeed = 1.f; // �ùķ��̼� ����ġ

			size_t MaxParticleCount = MAX_PARTICLE_COUNT;
		} MainData;

		struct Emission
		{
			float ParticlesPerSecond = 10.f;

			struct Burst
			{
				float TimePos = 0.f; // ó���� �ð�
				size_t Count = 30u; // ����� ��ƼŬ ��
				size_t Cycles = 1u; // ����Ʈ �ݺ� Ƚ��
				float Interval = 0.01f; // ����Ʈ ���� ����
				float Probability = 1.f; // 0 ~ 1 ������ Ȯ�� ���� ���
			};

			std::vector<Burst> Bursts;
		} EmissionData;

		struct Shape
		{
			enum class EShape
			{
				Sphere,
				Hemisphere,
				Cone,
				Donut,
				Box,
				Circle,
				Rectangle,
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
			float AngleInDegree = 25; // ���Ⱒ
			float Radius = 1; // ������
			float DountRadius = 0.2f; // ���� ������
			float ArcInDegree = 360.f; // ȣ ũ��, ���� ���
			float RadiusThickness = 0.f; // �Թ�ü�� ��� �κ� ���� ����� ������ 0 ~ 1f
			float Speed;
			float Spread;

			DirectX::SimpleMath::Vector3 Position;
			DirectX::SimpleMath::Vector3 Rotation;
			DirectX::SimpleMath::Vector3 Scale{ 1, 1, 1 };
		} ShapeData;

		struct VelocityOverLifetime
		{
			DirectX::SimpleMath::Vector3 Velocity = { 0, 0, 0 };
			DirectX::SimpleMath::Vector3 Orbital = { 0, 0, 0 };
			DirectX::SimpleMath::Vector3 Offset{ 0, 0, 0 };
			bool bIsUsed{ false };
		} VelocityOverLifetimeData;

		struct LimitVelocityOverLifetime
		{
			float Speed{ 1.f };
			float Dampen{ 0.f }; // ���� �ӵ� �ʰ� �� ���� ����
			bool bIsUsed{ false };
		} LimitVelocityOverLifetimeData;

		struct ForceOverLifetime
		{
			DirectX::SimpleMath::Vector3 Force{ 0, 0, 0 };
			bool bIsUsed{ false };
		} ForceOverLifeTimeData;

		struct ColorOverLifetime
		{
			enum { RATIO_SIZE = 8 };

			std::vector<DirectX::SimpleMath::Vector2> AlphaRatios{ RATIO_SIZE }; // value, ratio 
			size_t AlphaRatioCount{ 0 };
			std::vector<DirectX::SimpleMath::Vector4> ColorRatios{ RATIO_SIZE }; // { value }, ratio
			size_t ColorRatioCount{ 0 };
			bool bIsUsed{ false };
		} ColorOverLifetimeData;

		struct SizeOverLifetime
		{
			DirectX::SimpleMath::Vector2 PointA; // { ratio, size }
			DirectX::SimpleMath::Vector2 PointB;
			DirectX::SimpleMath::Vector2 PointC;
			DirectX::SimpleMath::Vector2 PointD;
			bool bIsUsed{ false };
		} SizeOverLifetimeData;

		struct RotationOverLifetime
		{
			float AngularVelocityInDegree;
			bool bIsUsed{ false };
		} RotationOverLifetimeData;

		struct Render
		{
			enum class ERenderMode
			{
				Billboard,
			};
			enum class EBlendMode
			{
				Additive,
				Subtractive,
				Moudulate
			};

			ERenderMode RenderMode = ERenderMode::Billboard;
			EBlendMode BlendMode = EBlendMode::Additive;
			std::string TexturePath = "./resource/example/texture/Particle00.png";
			bool bUseMultiplyAlpha = true;
			bool bUseAlphaClip = true;
			float AlphaClipThreshold = 0.1f;
		} RenderData;
	};

	struct DecalInfo
	{
		std::filesystem::path TextureBasePath = "";
		fq::common::Material MatrialData;
		DirectX::SimpleMath::Matrix Transform;
		DirectX::SimpleMath::Matrix TexTransform;
		unsigned int Layer = 0u; // ��Į �ڽ��� �׷����� ����, �������� ���߿� �׷���
		float NormalThresholdInRadian = 3.14f; // ��Į �ڽ��� ����� ��ü�� �븻 ������ ������ �ִ� ����
		bool bUseMultiplyAlpha = true;
		bool bUseAlphaClip = true;
		float AlphaClipThreshold = 0.1f;
		bool bUseDebugRender = true;
		DirectX::SimpleMath::Color DebugRenderColor = { 1, 0, 0, 1 };
	};

	namespace debug
	{
		struct SphereInfo
		{
			DirectX::BoundingSphere Sphere;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
		};

		struct SphereInfoEx
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float ArcInRadian = DirectX::XM_2PI;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
		};

		struct HemisphereInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float ArcInRadian = DirectX::XM_2PI;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
		};

		struct ConeInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float AngleInRadian = 25;
			float ArcInRadian = DirectX::XM_2PI;
			float Height;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
		};

		struct DountInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float ArcInRadian = DirectX::XM_2PI;
			float DountRadius;
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

		struct RingInfoEx
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 MajorAxis;
			DirectX::SimpleMath::Vector3 MinorAxis;
			float ArcInRadian = DirectX::XM_2PI;
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

	//// ---------------------------------------------------------------------
	////							Light Probe
	//// ---------------------------------------------------------------------
	//struct CubeProbe
	//{
	//	unsigned short Index;
	//	DirectX::SimpleMath::Vector3 Position;
	//};
};