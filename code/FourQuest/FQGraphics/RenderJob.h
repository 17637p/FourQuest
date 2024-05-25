#pragma once

#include <memory>
#include <vector>

#include <directxtk/SimpleMath.h>

#include "../FQCommon/EObjectRenderType.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class TerrainMesh;
	class Material;
	class TerrainMaterial;

	// job을 일반화시키거나 더 세부로 쪼개거나 결정해야함
	class IStaticMeshObject;
	class ISkinnedMeshObject;
	class ITerrainMeshObject;

	struct StaticMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<StaticMesh> StaticMesh;
		std::shared_ptr<Material> Material;
		const DirectX::SimpleMath::Matrix* TransformPtr;
		EObjectRenderType ObjectRenderType;
		float Alpha;
		bool bUseShadow;
		IStaticMeshObject* tempObject;
	};

	struct SkinnedMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<SkinnedMesh> SkinnedMesh;
		std::shared_ptr<Material> Material;
		const DirectX::SimpleMath::Matrix* TransformPtr;
		const std::vector<DirectX::SimpleMath::Matrix>* BoneMatricesPtr;
		EObjectRenderType ObjectRenderType;
		float Alpha;
		bool bUseShadow;
		ISkinnedMeshObject* tempObject;
	};

	struct TerrainMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<TerrainMesh> TerrainMesh;
		std::shared_ptr<TerrainMaterial> TerrainMaterial;
		const DirectX::SimpleMath::Matrix* TransformPtr;
		ITerrainMeshObject* tempObject;
	};
}