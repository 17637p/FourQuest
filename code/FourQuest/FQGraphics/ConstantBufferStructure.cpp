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

	// https://docs.unity3d.com/kr/2021.2/Manual/LightProbes-TechnicalInformation.html
	// https://www.ppsloan.org/publications/StupidSH36.pdf
	void ConstantBufferHelper::UpdateLightProbeCB(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<D3D11ConstantBuffer<LightProbe>>& cbuffer, float* r, float* g, float* b)
	{
		LightProbe probe;

		DirectX::SimpleMath::Vector4 vCoeff[3];
		float* fLight[3] = { r, g, b };

		static const float s_fSqrtPI = ((float)sqrtf(DirectX::XM_PI));
		const float fc0 = 1.0f / (2.0f * s_fSqrtPI);
		const float fc1 = (float)sqrt(3.0f) / (3.0f * s_fSqrtPI);
		const float fc2 = (float)sqrt(15.0f) / (8.0f * s_fSqrtPI);
		const float fc3 = (float)sqrt(5.0f) / (16.0f * s_fSqrtPI);
		const float fc4 = 0.5f * fc2;

		int iC;
		for (iC = 0; iC < 3; iC++)
		{
			vCoeff[iC].x = -fc1 * fLight[iC][3];
			vCoeff[iC].y = -fc1 * fLight[iC][1];
			vCoeff[iC].z = fc1 * fLight[iC][2];
			vCoeff[iC].w = fc0 * fLight[iC][0] - fc3 * fLight[iC][6];
		}

		probe.Ar = vCoeff[0];
		probe.Ag = vCoeff[1];
		probe.Ab = vCoeff[2];

		for (iC = 0; iC < 3; iC++)
		{
			vCoeff[iC].x = fc2 * fLight[iC][4];
			vCoeff[iC].y = -fc2 * fLight[iC][5];
			vCoeff[iC].z = 3.0f * fc3 * fLight[iC][6];
			vCoeff[iC].w = -fc2 * fLight[iC][7];
		}

		probe.Br = vCoeff[0];
		probe.Bg = vCoeff[1];
		probe.Bb = vCoeff[2];

		vCoeff[0].x = fc4 * fLight[0][8];
		vCoeff[0].y = fc4 * fLight[1][8];
		vCoeff[0].z = fc4 * fLight[2][8];
		vCoeff[0].w = 1.0f;

		probe.C = vCoeff[0];

		cbuffer->Update(device, probe);
	}

}