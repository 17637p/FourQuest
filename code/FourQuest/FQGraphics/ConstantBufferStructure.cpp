#include "ConstantBufferStructure.h"

#include <IFQRenderObject.h>

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
		std::shared_ptr<D3D11ConstantBuffer<CBMaterial>>& cbuffer,
		const std::shared_ptr<Material>& material)
	{
		CBMaterial CBMaterialData;
		CBMaterialData.BaseColor = material->GetMaterialData().BaseColor;
		CBMaterialData.Metalness = material->GetMaterialData().Metalness;
		CBMaterialData.Roughness = material->GetMaterialData().Roughness;

		CBMaterialData.bUseAlbedoMap = material->GetHasBaseColor() && material->GetTextureUseFlag().bUseBaseColor;
		CBMaterialData.bUseMetalnessMap = material->GetHasMetalness() && material->GetTextureUseFlag().bUseMetalness;
		CBMaterialData.bUseNormalMap = material->GetHasNormal() && material->GetTextureUseFlag().bUseNormalness;
		CBMaterialData.bUseRoughnessMap = material->GetHasRoughness() && material->GetTextureUseFlag().bUseRoughness;
		CBMaterialData.bUseEmissiveMap = material->GetHasEmissive();
		CBMaterialData.bUseOpacityMap = material->GetHasOpacity();

		cbuffer->Update(device, CBMaterialData);
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
		const std::shared_ptr<TerrainMaterial>& material,
		const ITerrainMeshObject* iTerrainObject)
	{
		TerrainTexture terrainTexture;
		terrainTexture.NumOfTexture = material->GetNumOfTexture();

		for (unsigned short i = 0; i < terrainTexture.NumOfTexture; i++)
		{
			terrainTexture.layer[i].TileSizeX = material->GetTileSizeX(i);
			terrainTexture.layer[i].TileSizeY = material->GetTileSizeY(i);
			terrainTexture.layer[i].TileOffsetX = material->GetTileOffsetX(i);
			terrainTexture.layer[i].TileOffsetY = material->GetTileOffsetY(i);

			terrainTexture.layer[i].Metalic = material->GetMetalic(i);
			terrainTexture.layer[i].Roughness = material->GetRoughness(i);
		}

		int a = sizeof(TerrainTexture);

		cbuffer->Update(device, terrainTexture);
	}

}