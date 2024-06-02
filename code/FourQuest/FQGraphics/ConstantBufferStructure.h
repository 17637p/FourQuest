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
			int unused[1];
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
		} ShapeData;
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