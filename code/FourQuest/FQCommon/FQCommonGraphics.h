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
		float spot = 1; //Todo: 아마도 spot 지름인거 같은데 확인해보고 바꾸기

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
		std::string AnimationName; // Model Data의 Animation 이름
		std::string AnimationKey; // ISkinnedMeshObject SetAnimation에서 사용할 이름
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
		//unsigned short NumOfTexture; // 일단 최대 4

		//std::vector<std::wstring> BaseColorFileNames;
		//std::vector<std::wstring> MetalnessFileNames;
		//std::vector<std::wstring> RoughnessFileNames;
		//std::vector<std::wstring> NormalFileNames;

		std::vector<TerrainLayer> Layers; // 최대 4
		std::string AlPhaFileName; // R에는 BaseColor1, G에는 2, B에는 3, A에는 4
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
			float Duration = 5; // 총 재생시간
			bool bIsLooping = true;

			EOption StartDelayOption = EOption::Constant;
			float StartDelay[2] = { 0.f, }; // 방출 시작 시간

			EOption StartLifeTimeOption = EOption::Constant;
			float StartLifeTime[2] = { 5.f, }; // 파티클 생존 시간

			EOption StartSpeedOption = EOption::Constant;
			float StartSpeed[2] = { 5.f, }; // 방출 속도

			EOption StartSizeOption = EOption::Constant;
			float StartSize[2] = { 1.f, }; // 파티클 크기

			EOption StartRotationOption = EOption::Constant;
			float StartRotation[2] = { 0.f, }; // 파티클 회전

			EOption StartColorOption = EOption::Constant;
			DirectX::SimpleMath::Color StartColor[2] = { { 1.f, 1.f, 1.f, 1.f }, }; // 파티클 색상

			EOption GravityModifierOption = EOption::Constant;
			float GravityModifier[2] = { 0.f, }; // 파티클 중력

			float SimulationSpeed = 1.f; // 시뮬레이션 가중치

			size_t MaxParticleCount = MAX_PARTICLE_COUNT;
		} MainModuleData;

		struct Emission
		{
			float ParticlesPerSecond = 10.f;

			struct Burst
			{
				float TimePos = 0.f; // 처리할 시간
				size_t Count = 30u; // 방출될 파티클 수
				size_t Cycles = 1u; // 버스트 반복 횟수
				float Interval = 0.01f; // 버스트 사이 간격
				float Probability = 1.f; // 0 ~ 1 사이의 확률 값을 사용
			};

			std::vector<Burst> Bursts;
		} EmissionModuleData;

		struct Shape
		{
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

			EShape ShapeType = EShape::Sphere; // 방출 모양
			EMode ModeType = EMode::Random; // 방출 방식
			float Angle = 25; // 
			float Radius = 1; // 반지름
			float DountRadius = 0.2f; // 도넛 반지름
			float Arc = 360.f; // 호 크기, 각도 사용
			float RadiusThickness = 0.f; // 입방체의 어느 부분 부터 방출될 것인지 0 ~ 1f
			float Speed;
			float Spread;
		} ShapeData;

		struct VelocityOverLifetime {};
		struct ColorOverLifetime {};
		struct SizeOverLifetime {};
		struct RotationOverLifetime {};
		struct TextureSheetAnimation {};

		struct RenderModule
		{
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
		} RenderModuleData;
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