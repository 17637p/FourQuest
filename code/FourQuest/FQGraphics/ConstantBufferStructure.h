#pragma once

#include "CommonHeader.h"

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
		DirectX::SimpleMath::Matrix ShadowViewProjTexMat;
	};

	struct BoneTransform
	{
		enum { MAX_BOND_COUNT = 128 };
		DirectX::SimpleMath::Matrix FinalTransforms[128];
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