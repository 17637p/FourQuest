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
		const auto& info = material->GetInfo();

		CBMaterial CBMaterialData;
		CBMaterialData.BaseColor = info.BaseColor;
		CBMaterialData.EmissiveColor = info.EmissiveColor;
		CBMaterialData.TexTransform = DirectX::SimpleMath::Matrix::CreateScale(info.Tiling.x, info.Tiling.y, 0) * DirectX::SimpleMath::Matrix::CreateTranslation(info.Offset.x, info.Offset.y, 0);
	
		CBMaterialData.Metalness = info.Metalness;
		CBMaterialData.Roughness = info.Roughness;
		CBMaterialData.bUseAlbedoMap = material->GetHasBaseColor() && info.bUseBaseColor;
		CBMaterialData.bUseMetalnessMap = material->GetHasMetalness() && info.bUseMetalness;
		
		CBMaterialData.bUseNormalMap = material->GetHasNormal() && info.bUseNormalness;
		CBMaterialData.bUseRoughnessMap = material->GetHasRoughness() && info.bUseRoughness;
		CBMaterialData.bUseEmissiveMap = material->GetHasEmissive();
		
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
		const std::shared_ptr<ITerrainMeshObject> iTerrainObject)
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