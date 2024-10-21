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

	enum class ELightMode
	{
		Realtime,
		Mixed,
		Baked //
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

		ELightMode mode = ELightMode::Realtime;
	};

	struct MeshObjectInfo
	{
		bool bUseShadow = true;
		bool bUseLightProbe;
		DirectX::SimpleMath::Color OutlineColor;
		bool bIsAppliedDecal = false;

		enum class EObjectType
		{
			Static,
			Dynamic
		} ObjectType = EObjectType::Dynamic;
	};

	struct MaterialInstanceInfo
	{
		bool bUseInstanceing = false;

		bool bUseInstanceAlpha = false;
		float Alpha = 1.f;
		bool bUseDissolveCutoff = false;
		float DissolveCutoff = 0.f;

		bool bUseRimLight = false;
		DirectX::SimpleMath::Color RimLightColor;
		float RimPow = 2.f;
		float RimIntensity = 1.f;

		bool bUseInvRimLight = false;
		DirectX::SimpleMath::Color InvRimLightColor;
		float InvRimPow = 2.f;
		float InvRimIntensity = 1.f;

		bool bUseUVScaleOffset = false;
		DirectX::SimpleMath::Vector2 UVScale = { 1, 1 };
		DirectX::SimpleMath::Vector2 UVOffset = { 0, 0 };

		bool bUseBaseColor = false;
		DirectX::SimpleMath::Color BaseColor;

		bool bUseEmissiveColor = false;
		DirectX::SimpleMath::Color EmissiveColor;

		bool bUseBlendUV = false;
		DirectX::SimpleMath::Vector4 BlendUVScaleOffset;
	};

	struct UIInfo
	{
		float StartX = 0.f; // 좌상단 점 위치 
		float StartY = 0.f;
		float Width = 100.f;
		float Height = 100.f;

		float Alpha = 1.f; // 1이 불투명 0이 투명 

		unsigned int Layer = 0; // 작을 수록 위에 클 수록 아래에 출력

		float XRatio = 1.f; // 1이 전부다 출력
		float YRatio = 1.f; // 1이 전부다 출력

		std::string ImagePath = "";

		// 둘 중 하나만 사용할 것
		std::string MaskPath = ""; // "" 이면 마스크 없음, 아니면 마스크 있음으로 처리
		float fillDegree = -1; // 0 보다 크면 사용 

		// 선택 사항
		float RotationAngle = 0.f;
		float ScaleX = 1.f;
		float ScaleY = 1.f;

		bool isCenter = false;
		bool isRender = true;

		DirectX::SimpleMath::Color Color; // Color가 있으면 네모 상자 그리기
	};

	struct SpriteInfo
	{
		float StartX = 0.f; // 좌상단 점 위치 
		float StartY = 0.f;
		float Width = 100.f;
		float Height = 100.f;

		float ScaleX = 1.f;
		float ScaleY = 1.f;

		std::string ImagePath = "";
		unsigned int Layer = 0; // 작을 수록 위에 클 수록 아래에 출력

		float ImageNum = 5;
		float Speed = 0.5f;
		float CurImage = 0;
		float CurTime = 0;

		bool isCenter = false;
		bool isRender = true;
	};

	enum class ETextAlign
	{
		LeftTop,
		LeftCenter,
		LeftBottom,
		CenterTop,
		CenterCenter,
		CenterBottom,
		RightTop,
		RightCenter,
		RightBottom
	};

	enum class ETextBoxAlign
	{
		LeftTop,
		CenterCenter,
	};

	struct TextInfo
	{
		std::string Text = ""; // 실제 띄울 내용

		int CenterX = 50;
		int CenterY = 50;
		int Width = 100;
		int Height = 100;

		float ScaleX = 1;
		float ScaleY = 1;

		std::string FontPath = "Verdana";
		int FontSize = 10;
		DirectX::SimpleMath::Color FontColor = { 0, 0, 0, 1 };

		ETextAlign Align = ETextAlign::LeftTop;
		ETextBoxAlign BoxAlign = ETextBoxAlign::CenterCenter;

		unsigned int Layer = 0; // 작을 수록 위에 클 수록 아래에 출력

		bool IsRender = true;
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
		std::vector<TerrainLayer> Layers; // 최대 4
		std::string AlPhaFileName; // R에는 BaseColor1, G에는 2, B에는 3, A에는 4

		std::string HeightFileName; // Raw 파일
		float TextureWidth;
		float TextureHeight;

		float HeightScale; // 전체 높이 (Length)
		float TerrainWidth; // 가로 크기
		float TerrainHeight; // 세로 크기
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
			DirectX::SimpleMath::Vector2 StartDelay{ 0.f, 0.f }; // 방출 시작 시간

			EOption StartLifeTimeOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartLifeTime{ 5.f, 5.f }; // 파티클 생존 시간

			EOption StartSpeedOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartSpeed{ 5.f, 5.f }; // 방출 속도

			EOption StartSizeOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartSize{ 1.f, 1.f }; // 파티클 크기

			EOption StartRotationOption = EOption::Constant;
			DirectX::SimpleMath::Vector2  StartRotation{ 0.f, 0.f }; // 파티클 회전

			EOption StartColorOption = EOption::Constant;
			DirectX::SimpleMath::Color StartColor0{ 1.f, 1.f, 1.f, 1.f }; // 파티클 색상
			DirectX::SimpleMath::Color StartColor1{ 1.f, 1.f, 1.f, 1.f }; // 파티클 색상

			EOption GravityModifierOption = EOption::Constant;
			DirectX::SimpleMath::Vector2 GravityModifier{ 0.f, 0.f }; // 파티클 중력

			float SimulationSpeed = 1.f; // 시뮬레이션 가중치

			size_t MaxParticleCount = MAX_PARTICLE_COUNT;
		} MainData;

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

			EShape ShapeType = EShape::Sphere; // 방출 모양
			EMode ModeType = EMode::Random; // 방출 방식
			float AngleInDegree = 25; // 방출각
			float Radius = 1; // 반지름
			float DountRadius = 0.2f; // 도넛 반지름
			float ArcInDegree = 360.f; // 호 크기, 각도 사용
			float RadiusThickness = 0.f; // 입방체의 어느 부분 부터 방출될 것인지 0 ~ 1f
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
			float Dampen{ 0.f }; // 제한 속도 초과 시 감소 비율
			bool bIsUsed{ false };
		} LimitVelocityOverLifetimeData;

		struct ForceOverLifetime
		{
			DirectX::SimpleMath::Vector3 Force{ 0, 0, 0 };
			bool bIsUsed{ false };
		} ForceOverLifeTimeData;

		struct ColorOverLifetime
		{
			std::vector<DirectX::SimpleMath::Vector2> AlphaRatios{ { 1, 0 }, }; // value, ratio 
			std::vector<DirectX::SimpleMath::Vector4> ColorRatios{ { 1, 1, 1, 0}, }; // { value }, ratio
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
				// StretchedBillboard,
				// HorizontalBillboard,
				// VerticalBillboard,
				// Mesh,
				// None 
			};

			ERenderMode RenderMode = ERenderMode::Billboard;
		} RenderData;

		struct Instance
		{
			bool bIsEmit{ true };
			bool bIsReset{ true };
			bool bUseMultiplyAlpha{ true };
			DirectX::SimpleMath::Color DebugRenderColor = { 1, 0, 0, 1 };;
			bool bIsRenderDebug{ true };
		} InstanceData;

		struct Sprite
		{
			unsigned int WidthCount{ 1 };
			unsigned int HeightCount{ 1 };
			unsigned int FrameCount{ 1 };
			float FramePerSecond{ 0.1 };
			bool bIsLooping{ false };
			bool bIsUsed{ false };
		} SpriteData;
	};

	struct DecalInfo
	{
		float Width = 1.f;
		float Height = 1.f;
		float Depth = 1.f;
		DirectX::SimpleMath::Vector3 Pivot = { 0, 0, 0 };
		float DecalRotation = 0.f;

		float NormalThresholdInDegree = 180.f; // 데칼 박스의 방향과 물체의 노말 사이의 랜더링 최대 각도

		DirectX::SimpleMath::Vector2 Tiling = { 1, 1 };
		DirectX::SimpleMath::Vector2 Offset = { 0, 0 };
		float Rotation = 0.f;

		DirectX::SimpleMath::Color DebugRenderColor = { 1, 0, 0, 1 };
		bool bIsRenderDebug = true;

		bool bIsIgnoreParentRotation = false;
	};

	struct TrailInfo
	{
		enum { MAX_VERTEX_SIZE = 1024 };

		enum class EAlignment
		{
			View,
			TransformZ,
		};

		enum class ETextureMode
		{
			Stretch, // 트레일 전체 길이를 한 번에 매핑
			Tile, // 월드 공간의 단위를 기반하여 텍스처 반복, 반복 거리 10이네 
			DistributePerSegment,// 버텍스 간격이 균등하다고 가정한 후 트레일의 전체 길이를 한 번에 매핑
			RepeatPerSegment, // 트레일 세그먼트당 한 번 트레일 텍스처 반복
		};

		float Time = 5.f;
		std::vector<DirectX::SimpleMath::Vector2> WidthRatios = { {10, 0 } }; // x : width, y : ratio
		float MinVertexDistance = 1.f; // 월드 공간에 정의된 트레일 간 최소 거리
		size_t VertexDivisionCount = 1; // 두 정점 사이에 곡선으로 나눌 개수
		bool bIsEmit = true;
		bool bIsReset = true;
		std::vector<DirectX::SimpleMath::Vector4> ColorRatios{ { 1, 1, 1, 0} }; // xyz : rgb, z : ratio
		std::vector<DirectX::SimpleMath::Vector2> AlphaRatios{ { 1, 0 } }; // x : a, y : ratio

		EAlignment AlignmentType = EAlignment::View;
		ETextureMode TextureMode = ETextureMode::DistributePerSegment;
	};

	namespace debug
	{
		struct SphereInfo
		{
			DirectX::BoundingSphere Sphere;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct SphereInfoEx
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float ArcInRadian = DirectX::XM_2PI;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct HemisphereInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 XAxis = DirectX::SimpleMath::Vector3::UnitX;
			DirectX::SimpleMath::Vector3 YAxis = DirectX::SimpleMath::Vector3::UnitY;
			DirectX::SimpleMath::Vector3 ZAxis = DirectX::SimpleMath::Vector3::UnitZ;
			float ArcInRadian = DirectX::XM_2PI;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
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
			bool bUseDepthTest = true;
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
			bool bUseDepthTest = true;
		};

		struct AABBInfo
		{
			DirectX::BoundingBox AABB;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct OBBInfo
		{
			DirectX::BoundingOrientedBox OBB;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct FrustumInfo
		{
			DirectX::BoundingFrustum Frustum;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
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
			bool bUseDepthTest = true;
		};

		struct RingInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 MajorAxis;
			DirectX::SimpleMath::Vector3 MinorAxis;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct RingInfoEx
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 MajorAxis;
			DirectX::SimpleMath::Vector3 MinorAxis;
			float ArcInRadian = DirectX::XM_2PI;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct RayInfo
		{
			DirectX::SimpleMath::Vector3 Origin;
			DirectX::SimpleMath::Vector3 Direction;
			bool Normalize = true;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};

		struct PolygonInfo
		{
			std::vector<DirectX::SimpleMath::Vector3> Points;
			DirectX::SimpleMath::Color Color = { 1.f, 1.f, 1.f, 1.f };
			bool bUseDepthTest = true;
		};
	};

	enum ESampleMode
	{
		Clamp,
		Wrap,
		Border,
	};

	enum ERasterizeMode
	{
		BackFaceClip,
		TwoSide,
	};

	enum class EDissolveOperator
	{
		Additive,
		Subtractive,
		Modulate
	};

	// material
	struct MaterialInfo
	{
		// 쉐이더 외부 분기
		enum class ERenderMode
		{
			Opaque,
			Transparent
		} RenderModeType = ERenderMode::Opaque;

		ESampleMode SampleType = ESampleMode::Wrap;
		ERasterizeMode RasterizeType = ERasterizeMode::BackFaceClip;

		// 쉐이더 내의 분기
		bool bUseBaseColor = true;
		DirectX::SimpleMath::Color BaseColor = { 1.f, 1.f, 1.f, 1.f };
		std::wstring BaseColorFileName;

		bool bUseMetalness = true;
		float Metalness = 0.f;
		std::wstring MetalnessFileName;

		bool bUseRoughness = true;
		float Roughness = 0.f;
		std::wstring RoughnessFileName;

		bool bIsUsedEmissive = true;
		DirectX::SimpleMath::Color EmissiveColor = { 0.f, 0.f, 0.f, 0.f };
		std::wstring EmissiveFileName;
		float EmissiveIntensity = 1.f;

		bool bUseNormalness = true;
		std::wstring NormalFileName;

		bool bIsUsedMetalnessSmoothness = false;
		std::wstring MetalnessSmoothnessFileName;

		DirectX::SimpleMath::Vector2 Tiling = { 1, 1 };
		DirectX::SimpleMath::Vector2 Offset = { 0, 0 };
		float AlphaCutoff = 0.1f;

		// 인자 자체가 구체 타입으로 늘어난다면?
		bool bUseDissolve = false;
		std::wstring NoiseFileName;
		float OutlineThickness = 1.15;
		float DissolveCutoff = -1;
		DirectX::SimpleMath::Color DissolveStartColor = { 1, 1, 1, 1 };
		DirectX::SimpleMath::Color DissolveEndColor = { 0, 0, 0, 0 };
		DirectX::SimpleMath::Color DissolveStartEmissive = { 0, 0, 0, 0 };
		DirectX::SimpleMath::Color DissolveEndEmissive = { 0, 0, 0, 0 };

		bool bUseRimLight = false;
		DirectX::SimpleMath::Color RimLightColor;
		float RimPow = 2.f;
		float RimIntensity = 1.f;

		bool bUseInvRimLight = false;
		DirectX::SimpleMath::Color InvRimLightColor;
		float InvRimPow = 2.f;
		float InvRimIntensity = 1.f;

		bool bUseMulEmissiveAlpha = false; // 이미시브 알파값 곱하기 여부

		bool bUseBlendTexture = false;
		std::wstring BlendTextureName;
		bool bIsBlendBaseColor = false;
		bool bIsBlendEmissive = false;
		DirectX::SimpleMath::Vector2 BlendTiling = { 1, 1 };
		DirectX::SimpleMath::Vector2 BlendOffset = { 0, 0 };
	};

	struct ParticleMaterialInfo
	{
		enum class ERenderMode
		{
			Additive,
			Subtractive,
			Modulate,
			AlphaBlend,
		} RenderModeType = ERenderMode::Additive;

		enum class EColorMode
		{
			Multiply,
			Additive,
			Subtractive,
			Overlay, // 
			Color, // 입자 텍스처 알파와 입자의 알베도 색상
			Difference // 
		} ColorModeType = EColorMode::Multiply;

		DirectX::SimpleMath::Color BaseColor = { 1.f, 1.f, 1.f, 1.f };
		DirectX::SimpleMath::Color EmissiveColor = { 0.f, 0.f, 0.f, 0.f };

		float EmissiveIntensity = 1.f;

		std::wstring BaseColorFileName;
		std::wstring EmissiveFileName;

		bool bIsUsedBaseColor = true;
		bool bIsUsedEmissive = true;

		DirectX::SimpleMath::Vector2 Tiling = { 1, 1 };
		DirectX::SimpleMath::Vector2 Offset = { 0, 0 };

		float AlphaCutoff = 0.1f;
		bool bIsTwoSide = false;
		bool bUseMultiplyAlpha = true;
	};

	struct DecalMaterialInfo
	{
		DirectX::SimpleMath::Color BaseColor = { 1.f, 1.f, 1.f, 1.f };
		DirectX::SimpleMath::Color EmissiveColor = { 0.f, 0.f, 0.f, 1.f };

		std::wstring BaseColorFileName;
		std::wstring NormalFileName;
		std::wstring EmissiveFileName;

		bool bUseBaseColor = true;
		bool bUseNormalness = true;
		bool bIsUsedEmissive = true;

		float NormalBlend = 0.5f; // 0 ~ 1, srcNormal의 가중치, 1이라면 decal의 노말이 전부 적용된다.
		float AlphaCutoff = 0.1f;
		
		bool bUseEmissiveBlend = false; // 이미시브 블렌딩 사용 여부
	};

	//// ---------------------------------------------------------------------
	////							Light Probe
	//// ---------------------------------------------------------------------
	//struct CubeProbe
	//{
	//	unsigned short Index;
	//	DirectX::SimpleMath::Vector3 Position;
	//};

	struct PostProcessingInfo
	{
		float Gamma = 2.2f;

		// color adjustment
		float Exposure = 1.f; // 2 제곱하여 적용
		float Contrast = 0.f; // * 0.01 + 1하여 적용
		float Saturation = 0.f; // * 0.01 + 1하여 적용
		bool bUseColorAdjustment = false;

		// bloom
		float BloomIntensity = 1.f; // 최종 연산 시 이미지 합성 강도
		float BloomThreshold = 1.f; // 블룸 시작 밝기
		float BloomScatter = 1.f; // 블러링 된 이미지 누적 시 합성 강도
		DirectX::SimpleMath::Color BloomColorTint = { 1, 1, 1, 1 };
		bool bUseBloomScatter = false;
		bool bUseBloom = false;

		// split toning;
		DirectX::SimpleMath::Color ShadowColor = { 128, 128, 128, 0 };
		DirectX::SimpleMath::Color HighlightColor = { 128, 128, 128, 0 };
		float Balance = 0.f; // ~1 ~ 1, 음수에 가까울 수록 Shadow Color 영향을 많이 받음
		bool bUseSplitToning = false;

		// vignett
		DirectX::SimpleMath::Color VignettColor = { 0, 0, 0, 1 };
		float VignettRadius = 0.8f;
		float VignettSmoothness = 0.4f;
		bool bUseVignett = false;

		// toneMapping
		// operator
		bool bUseToneMapping = true;

		// Fog
		bool bUseFog = false;
		DirectX::SimpleMath::Vector4 fogColor = { 0.5f, 0.5f, 0.5f, 1.0f }; // 안개 색상 
		float fogVisibleArea = 200.0f; // 가시 영역 near ~ far 기준으로 값을 넣어야 함 near가 1 far가 100이면 20일때 20퍼 보인다는 뜻

		// SSR
		bool bUseSSR = false;
		float max_iteration = 160;
		float max_thickness = 0.00001;

		bool bUseGrayScale = false;

		// Color Blend
		bool bUseBlendColor = false;
		DirectX::SimpleMath::Color BlendColor = { 0, 0, 0, 0 };

		// Texture Blend
		bool bUseBlendTexture = false;
		DirectX::SimpleMath::Color BlendTextureColor = { 1, 1, 1, 1 };
		std::wstring BlendTextureName;
	};

	struct DebugInfo
	{
		size_t StaticMeshObjectCount;
		size_t SkinnedMeshObjectCount;

		size_t StaticMeshObjectVertexCount;
		size_t SkinnedMeshObjectVertexCount;

		size_t StaticMeshObjectPolygonCount;
		size_t SkinnedMeshObjectPolygonCount;

		size_t CullingStaticMeshObjectCount;
		size_t CullingSkinnedMeshObjectCount;

		size_t CullingStaticMeshObjectVertexCount;
		size_t CullingSkinnedMeshObjectVertexCount;

		size_t CullingStaticMeshObjectPolygonCount;
		size_t CullingSkinnedMeshObjectPolygonCount;
	};
};