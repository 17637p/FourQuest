#include "ConstantBufferStructure.h"

#include "D3D11Device.h"
#include "D3D11Buffer.h"
#include "Material.h"
#include "D3D11Util.h"

namespace fq::graphics
{
	void ConstantBufferHelper::UpdateModelTransformCB(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ConstantBuffer<ModelTransform>>& cbuffer,
		const DirectX::SimpleMath::Matrix& transform)
	{
		ModelTransform modelTransform;
		modelTransform.WorldMat = transform.Transpose();
		modelTransform.WorldInvTransposeMat = D3D11Util::InverseTranspose(transform).Transpose();
		cbuffer->Update(device, modelTransform);
	}
	void ConstantBufferHelper::UpdateModelTextureCB(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ConstantBuffer<ModelTexutre>>& cbuffer,
		const std::shared_ptr<Material>& material)
	{
		ModelTexutre modelTexture;
		modelTexture.bUseAlbedoMap = material->GetHasBaseColor();
		modelTexture.bUseMetalnessMap = material->GetHasMetalness();
		modelTexture.bUseNormalMap = material->GetHasNormal();
		modelTexture.bUseRoughnessMap = material->GetHasRoughness();
		modelTexture.bUseEmissiveMap = material->GetHasEmissive();
		modelTexture.bUseOpacityMap = material->GetHasOpacity();

		cbuffer->Update(device, modelTexture);
	}
	void ConstantBufferHelper::UpdateBoneTransformCB(const std::shared_ptr<D3D11Device>& device,
		std::shared_ptr<D3D11ConstantBuffer<BoneTransform>>& cbuffer,
		const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms)
	{
		size_t matrixCount = std::min<size_t>(finalTransforms.size(), (size_t)BoneTransform::MAX_BOND_COUNT);

		BoneTransform boneTransform;
		memcpy(boneTransform.FinalTransforms, finalTransforms.data(), sizeof(DirectX::SimpleMath::Matrix) * matrixCount);

		cbuffer->Update(device, boneTransform);
	}
	void ConstantBufferHelper::UpdateTerrainTextureCB(const std::shared_ptr<D3D11Device>& device, 
		std::shared_ptr<D3D11ConstantBuffer<TerrainTexture>>& cbuffer, 
		const std::shared_ptr<TerrainMaterial>& material)
	{
		TerrainTexture terrainTexture;
		terrainTexture.NumOfTexture = material->GetNumOfTexture();

		cbuffer->Update(device, terrainTexture);
	}

}