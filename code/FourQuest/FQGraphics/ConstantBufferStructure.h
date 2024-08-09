#pragma once

#include "CommonHeader.h"
#include "Light.h"

namespace fq::graphics
{
	struct ModelTransform
	{
		DirectX::SimpleMath::Matrix WorldMat;
		DirectX::SimpleMath::Matrix WorldInvTransposeMat;
	};

	struct LightMapInfomation
	{
		DirectX::SimpleMath::Vector4 UVScaleOffset;
		unsigned int UVIndex;
		int bUseLightmap;
		int bUseDirection;
		float unused[1];
	};

	struct ViewProjectionMatrix
	{
		DirectX::SimpleMath::Matrix ViewMatrix;
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct SceneTrnasform
	{
		DirectX::SimpleMath::Matrix ViewMat;
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct SceneInfomation
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
		DirectX::SimpleMath::Vector4 EyePosition;
	};

	struct BoneTransform
	{
		enum { MAX_BOND_COUNT = 128 };
		DirectX::SimpleMath::Matrix FinalTransforms[MAX_BOND_COUNT];
	};

	struct DirectionalShadowInfo
	{
		enum { CASCADE_COUNT = 3 };
		enum { MAX_SHADOW_COUNT = 3 };

		DirectX::SimpleMath::Matrix ShadowViewProj[CASCADE_COUNT * MAX_SHADOW_COUNT];
		DirectX::SimpleMath::Vector4 CascadeEnds[MAX_SHADOW_COUNT];
		int ShadowCount;
		int LightModes[3];
	};

	struct CBMaterialInstance
	{
		int bUseInstanceAlpha;
		float Alpha;
		int bUseDissolveCutoff;
		float DissolveCutoff;
	};

	struct ViewRotationProjectionMatrix
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct Layer
	{
		float TileSizeX;
		float TileSizeY;
		float TileOffsetX;
		float TileOffsetY;

		float Metalic;
		float Roughness;
		float a;
		float b;
	};

	struct TerrainTexture
	{
		int NumOfTexture;
		int pad[3]; // 16

		Layer layer[4]; // 128
	};

	struct FrustumCorners
	{
		DirectX::SimpleMath::Vector4 FrustumFarCorners[4];
	};

	struct LightData
	{
		DirectionalLight directionalLight[3];
		PointLight pointLight[10];
		SpotLight spotLight[5];

		unsigned int numOfDirectionalLight;
		unsigned int numOfPointLight;
		unsigned int numOfSpotLight;

		unsigned int isUseIBL;

		DirectX::SimpleMath::Vector3 eyePosition;
		float pad2;
	};

	struct OutLineColor
	{
		DirectX::SimpleMath::Color color;
	};

	struct ScreenSize
	{
		unsigned int width;
		unsigned int height;
		float pad[2];
	};

	struct TerrainHull
	{
		float MinDist;
		float MaxDist;
		float MinTess;
		float MaxTess;
		DirectX::XMFLOAT3 gEyePosW;
		float pad;
		DirectX::XMFLOAT4 WorldFrustumPlanes[6];
	};

	struct CBParticleMaterial
	{
		DirectX::SimpleMath::Vector4 BaseColor;
		DirectX::SimpleMath::Vector4 EmissiveColor;
		DirectX::SimpleMath::Matrix TexTransform;

		int RenderMode;
		int ColorMode;
		int bUseAlbedoMap;
		int bUseEmissiveMap;

		int bUseMultiplyAlpha;
		float AlphaCutoff;
		float unused[2];
	};

	struct CBMaterial
	{
		DirectX::SimpleMath::Matrix TexTransform;
		DirectX::SimpleMath::Vector4 BaseColor;
		DirectX::SimpleMath::Vector4 EmissiveColor;
		DirectX::SimpleMath::Vector4 DissolveStartColor;
		DirectX::SimpleMath::Vector4 DissolveEndColor;

		float Metalness;
		float Roughness;
		int bUseAlbedoMap;
		int bUseMetalnessMap;

		int bUseRoughnessMap;
		int bUseNormalMap;
		int bUseEmissiveMap;
		float AlphaCutoff;

		float EmissiveIntensity;
		int bUseMetalnessSmoothnessMap;
		int bUseDissolve;
		float OutlineThickness;

		float DissolveCutoff;
		int DissolveOperator;
		float unused[2];
	};

	struct CBDecalObject
	{
		DirectX::SimpleMath::Matrix TexTransform;
		DirectX::SimpleMath::Matrix World;
		DirectX::SimpleMath::Matrix View;
		DirectX::SimpleMath::Matrix Proj;
		DirectX::SimpleMath::Matrix InvWV;

		DirectX::SimpleMath::Vector2 Deproject;
		float NormalThresholdInRadian;
		float unused[1];
	};

	struct CBDecalMaterial
	{
		DirectX::SimpleMath::Color BaseColor;
		DirectX::SimpleMath::Color EmissiveColor;

		int bUseBaseColor;
		int bUseNormalness;
		int bIsUsedEmissive;
		float NormalBlend;

		float AlphaCutoff;
		float unused[3];
	};

	struct ParticleFrameData
	{
		DirectX::SimpleMath::Matrix ViewMatrix;
		DirectX::SimpleMath::Matrix ProjMatrix;
		DirectX::SimpleMath::Matrix InvProjMatrix;
		unsigned int ScreenWidth;
		unsigned int ScreenHeight;
		float unused[2];
	};

	struct ParticleObjectData
	{
		struct Instance
		{
			DirectX::SimpleMath::Matrix Transform;
			float TimePos;
			float FrameTime;
			int NumToEmit;
			float RandomSeed;
		} InstanceData;

		struct Main
		{
			DirectX::SimpleMath::Color StartColor[2]; // 32

			float StartLifeTime[2]; // 8
			float StartSpeed[2]; // 8

			float StartRotation[2]; // 8
			float StartSize[2]; // 8

			float GravityModfier[2]; // 8
			float SimulationSpeed; // 4
			int MaxParticleSize; // 4
		} MainData;

		struct Shape
		{
			DirectX::SimpleMath::Matrix Transform;
			DirectX::SimpleMath::Vector3 Position;
			float pad0; // 4
			DirectX::SimpleMath::Vector3 Rotation;
			float pad1; // 4
			DirectX::SimpleMath::Vector3 Scale;
			float pad2; // 4

			int ShapeType;
			int ModeType;
			float AngleInRadian;
			float Radius;

			float DountRadius;
			float ArcInRadian;
			float Speed;
			float Spread;

			float RadiusThickness;
			float unused[3];
		} ShapeData;

		struct VelocityOverLifetime
		{
			DirectX::SimpleMath::Vector3 Velocity; // 12
			float pad0; // 4
			DirectX::SimpleMath::Vector3 Orbital; // 12
			float pad1; // 4
			DirectX::SimpleMath::Vector3 Offset; // 12
			float pad2; // 4
			int bIsUsed; // 4
			float unused[3]; // 12
		} VelocityOverLifetimeData;

		struct LimitVelocityOverLifetime
		{
			float Speed;
			float Dampen; // 제한 속도 초과 시 감소 비율
			int bIsUsed{ false };
			float unused[1];
		} LimitVelocityOverLifetimeData;

		struct ForceOverLifeTime
		{
			DirectX::SimpleMath::Vector3 Force;
			int bIsUsed{ false };
		} ForceOverLifeTimeData;

		struct ColorOverLifetime
		{
			DirectX::SimpleMath::Vector4 ColorRatios[8];
			DirectX::SimpleMath::Vector4 AlphaRatios[8];
			int AlphaRatioCount;
			int ColorRatioCount;
			int bIsUsed{ false };
			float unused[1];
		} ColorOverLifetimeData;

		struct SizeOverLifetime
		{
			DirectX::SimpleMath::Vector2 PointA;
			DirectX::SimpleMath::Vector2 PointB;
			DirectX::SimpleMath::Vector2 PointC;
			DirectX::SimpleMath::Vector2 PointD;
			int bIsUsed{ false };
			float unused[3];
		} SizeOverLifetimeData;

		struct RotationOverLifetime
		{
			float AngularVelocityInRadian;
			int bIsUsed{ false };
			float unused[2];
		} RotationOverLifetimeData;

		CBParticleMaterial ParticleMaterialData;
	};

	struct LightProbeCB
	{
		DirectX::XMFLOAT4 Ar;
		DirectX::XMFLOAT4 Ag;
		DirectX::XMFLOAT4 Ab;
		DirectX::XMFLOAT4 Br;
		DirectX::XMFLOAT4 Bg;
		DirectX::XMFLOAT4 Bb;
		DirectX::XMFLOAT4 C;
		float Intensity;
		DirectX::XMFLOAT3 pad;
	};

	struct SpecularMapFilterSettingsCB
	{
		float roughness;
		float EnvScale;
		float padding[2];
	};

	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11Device;
	class Material;
	class TerrainMaterial;
	class ITerrainMeshObject;

	class ConstantBufferHelper
	{
	public:
		static void UpdateModelTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<ModelTransform>>& cbuffer,
			const DirectX::SimpleMath::Matrix& transform);
		static void UpdateModelTextureCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<CBMaterial>>& cbuffer,
			const std::shared_ptr<Material>& material,
			const DirectX::SimpleMath::Matrix& texTransform = DirectX::SimpleMath::Matrix::Identity);
		static void UpdateTerrainTextureCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>>& cbuffer,
			const std::shared_ptr<TerrainMaterial>& material,
			const ITerrainMeshObject* iTerrainObject);
		static void UpdateBoneTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<BoneTransform>>& cbuffer,
			const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms);
		static void UpdateLightProbeCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<LightProbeCB>>& cbuffer,
			float* r, float* g, float* b, float intensity);
		static void UpdateMaterialInstance(const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11ConstantBuffer<CBMaterialInstance>>& materialInstanceCB,
			const MaterialInstanceInfo& materialInstanceInfo);
	};
}