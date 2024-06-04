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

	struct ViewProjectionMatrix
	{
		DirectX::SimpleMath::Matrix ViewMatrix;
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct SceneTrnasform
	{
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
		DirectX::SimpleMath::Matrix FinalTransforms[128];
	};

	struct DirectionalShadowTransform
	{
		enum { CASCADE_COUNT = 3 };
		enum { MAX_SHADOW_COUNT = 3 };

		DirectX::SimpleMath::Matrix ShadowViewProj[CASCADE_COUNT * MAX_SHADOW_COUNT];
		int ShadowCount;
		int unused[3];
	};

	struct DirectionalShadowInfo
	{
		DirectX::SimpleMath::Matrix ShadowViewProj[DirectionalShadowTransform::CASCADE_COUNT * DirectionalShadowTransform::MAX_SHADOW_COUNT];
		DirectX::SimpleMath::Vector4 CascadeEnds[DirectionalShadowTransform::MAX_SHADOW_COUNT];
		int ShadowCount;
		float unused[3];
	};

	struct AlphaData
	{
		int bUseAlphaConstant;
		float Alpha;
		float unused[2];
	};

	struct ViewRotationProjectionMatrix
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct ModelTexutre
	{
		int bUseAlbedoMap;
		int bUseMetalnessMap;
		int bUseRoughnessMap;
		int bUseNormalMap;
		int bUseEmissiveMap;
		int bUseOpacityMap;
		int unused[2];
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

	struct Emitter
	{
		DirectX::SimpleMath::Vector4 InitPositionW;
		DirectX::SimpleMath::Vector4 ReadomPositionW;

		DirectX::SimpleMath::Vector4 InitVelocityW;
		DirectX::SimpleMath::Vector4 ReadomVelocityW;

		DirectX::SimpleMath::Vector4 InitSizeW;
		DirectX::SimpleMath::Vector4 ReadomSizeW;

		float InitLifeTime;
		float ReadomLifeTime;
		unsigned int ParticleCount;
		float DeltaTime;
		float TotalTime;
		float unused[3];
	};

	struct ParticleMain
	{
		DirectX::SimpleMath::Matrix Transform;

		DirectX::SimpleMath::Vector4 StartSize;
		DirectX::SimpleMath::Vector4 StartColor1;
		DirectX::SimpleMath::Vector4 StartColor2;
		DirectX::SimpleMath::Vector2 StartLifeTime;
		DirectX::SimpleMath::Vector2 StartSpeed;
		DirectX::SimpleMath::Vector2 StartRotation;
		DirectX::SimpleMath::Vector2 GravityModifier;

		int StartLifeTimeOption;
		int StartSpeedOption;
		int StartSizeOption;
		int StartRotationOption;

		int StartColorOption;
		int GravityModifierOption;
		int RandomSeed;
		float DeltaTime;

		float SimulationSpeed;
		unsigned int ParticleCount;
		float unused[2];
	};

	struct ParticleShape
	{
		DirectX::SimpleMath::Matrix Transform;

		int ShapeType;
		int ModeType;
		float Angle;
		float Radius;

		float DountRadius;
		float Arc;
		float Speed;
		float Spread;

		float RadiusThickness;
		float unused[3];
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
			std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>>& cbuffer,
			const std::shared_ptr<Material>& material);
		static void UpdateTerrainTextureCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>>& cbuffer,
			const std::shared_ptr<TerrainMaterial>& material,
			const ITerrainMeshObject* iTerrainObject);
		static void UpdateBoneTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<BoneTransform>>& cbuffer,
			const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms);
	};
}