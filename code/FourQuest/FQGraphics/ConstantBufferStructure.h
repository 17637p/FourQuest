#pragma once

#include "CommonHeader.h"
#include "Light.h"

namespace fq::graphics
{
	struct ModelTransfrom
	{
		DirectX::SimpleMath::Matrix WorldMat;
		DirectX::SimpleMath::Matrix WorldInvTransposeMat;
	};

	struct SceneTrnasform
	{
		DirectX::SimpleMath::Matrix ViewProjMat;
	};

	struct BoneTransform
	{
		enum { MAX_BOND_COUNT = 128 };
		DirectX::SimpleMath::Matrix FinalTransforms[128];
	};

	struct ShadowTransform
	{
		DirectX::SimpleMath::Matrix ShadowViewProj[3];
	};

	struct CascadeEnd
	{
		DirectX::SimpleMath::Vector4 CascadeEnds;
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

	struct SceneLight
	{
		struct Light
		{
			DirectX::SimpleMath::Vector4 Direction;
			DirectX::SimpleMath::Vector4 Intensity;
		} Lights[3];

		DirectX::SimpleMath::Vector4 EyePosition;
		int bUseIBL;
		int unused[3];
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

	template <typename T>
	class D3D11ConstantBuffer;
	class D3D11Device;
	class Material;

	class ConstantBufferHelper
	{
	public:
		static void UpdateModelTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<ModelTransfrom>>& cbuffer,
			const DirectX::SimpleMath::Matrix& transform);
		static void UpdateModelTextureCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>>& cbuffer,
			const std::shared_ptr<Material>& material);
		static void UpdateBoneTransformCB(const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<D3D11ConstantBuffer<BoneTransform>>& cbuffer,
			const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms);
	};
}