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
		float spot = 1; //Todo: �Ƹ��� spot �����ΰ� ������ Ȯ���غ��� �ٲٱ�

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
		float StartX = 0.f; // �»�� �� ��ġ 
		float StartY = 0.f;
		float Width = 100.f;
		float Height = 100.f;

		float Alpha = 1.f; // 1�� ������ 0�� ���� 

		unsigned int Layer = 0; // ���� ���� ���� Ŭ ���� �Ʒ��� ���

		float XRatio = 1.f; // 1�� ���δ� ���
		float YRatio = 1.f; // 1�� ���δ� ���

		std::string ImagePath = "";

		// �� �� �ϳ��� ����� ��
		std::string MaskPath = ""; // "" �̸� ����ũ ����, �ƴϸ� ����ũ �������� ó��
		float fillDegree = -1; // 0 ���� ũ�� ��� 

		// ���� ����
		float RotationAngle = 0.f;
		float ScaleX = 1.f;
		float ScaleY = 1.f;

		bool isCenter = false;
		bool isRender = true;

		DirectX::SimpleMath::Color Color; // Color�� ������ �׸� ���� �׸���
	};

	struct SpriteInfo
	{
		float StartX = 0.f; // �»�� �� ��ġ 
		float StartY = 0.f;
		float Width = 100.f;
		float Height = 100.f;

		float ScaleX = 1.f;
		float ScaleY = 1.f;

		std::string ImagePath = "";
		unsigned int Layer = 0; // ���� ���� ���� Ŭ ���� �Ʒ��� ���

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
		std::string Text = ""; // ���� ��� ����

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

		unsigned int Layer = 0; // ���� ���� ���� Ŭ ���� �Ʒ��� ���

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

		float NormalThresholdInDegree = 180.f; // ��Į �ڽ��� ����� ��ü�� �븻 ������ ������ �ִ� ����

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
			Stretch, // Ʈ���� ��ü ���̸� �� ���� ����
			Tile, // ���� ������ ������ ����Ͽ� �ؽ�ó �ݺ�, �ݺ� �Ÿ� 10�̳� 
			DistributePerSegment,// ���ؽ� ������ �յ��ϴٰ� ������ �� Ʈ������ ��ü ���̸� �� ���� ����
			RepeatPerSegment, // Ʈ���� ���׸�Ʈ�� �� �� Ʈ���� �ؽ�ó �ݺ�
		};

		float Time = 5.f;
		std::vector<DirectX::SimpleMath::Vector2> WidthRatios = { {10, 0 } }; // x : width, y : ratio
		float MinVertexDistance = 1.f; // ���� ������ ���ǵ� Ʈ���� �� �ּ� �Ÿ�
		size_t VertexDivisionCount = 1; // �� ���� ���̿� ����� ���� ����
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
		// ���̴� �ܺ� �б�
		enum class ERenderMode
		{
			Opaque,
			Transparent
		} RenderModeType = ERenderMode::Opaque;

		ESampleMode SampleType = ESampleMode::Wrap;
		ERasterizeMode RasterizeType = ERasterizeMode::BackFaceClip;

		// ���̴� ���� �б�
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

		// ���� ��ü�� ��ü Ÿ������ �þ�ٸ�?
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

		bool bUseMulEmissiveAlpha = false; // �̹̽ú� ���İ� ���ϱ� ����

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
			Color, // ���� �ؽ�ó ���Ŀ� ������ �˺��� ����
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

		float NormalBlend = 0.5f; // 0 ~ 1, srcNormal�� ����ġ, 1�̶�� decal�� �븻�� ���� ����ȴ�.
		float AlphaCutoff = 0.1f;
		
		bool bUseEmissiveBlend = false; // �̹̽ú� ���� ��� ����
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
		float Exposure = 1.f; // 2 �����Ͽ� ����
		float Contrast = 0.f; // * 0.01 + 1�Ͽ� ����
		float Saturation = 0.f; // * 0.01 + 1�Ͽ� ����
		bool bUseColorAdjustment = false;

		// bloom
		float BloomIntensity = 1.f; // ���� ���� �� �̹��� �ռ� ����
		float BloomThreshold = 1.f; // ��� ���� ���
		float BloomScatter = 1.f; // ���� �� �̹��� ���� �� �ռ� ����
		DirectX::SimpleMath::Color BloomColorTint = { 1, 1, 1, 1 };
		bool bUseBloomScatter = false;
		bool bUseBloom = false;

		// split toning;
		DirectX::SimpleMath::Color ShadowColor = { 128, 128, 128, 0 };
		DirectX::SimpleMath::Color HighlightColor = { 128, 128, 128, 0 };
		float Balance = 0.f; // ~1 ~ 1, ������ ����� ���� Shadow Color ������ ���� ����
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
		DirectX::SimpleMath::Vector4 fogColor = { 0.5f, 0.5f, 0.5f, 1.0f }; // �Ȱ� ���� 
		float fogVisibleArea = 200.0f; // ���� ���� near ~ far �������� ���� �־�� �� near�� 1 far�� 100�̸� 20�϶� 20�� ���δٴ� ��

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