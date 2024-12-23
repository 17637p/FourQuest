#pragma once

#include <memory>
#include <vector>

#include <directxtk/SimpleMath.h>
#include "IFQGraphics.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class TerrainMesh;
	class Material;
	class TerrainMaterial;

	// job을 일반화시키거나 더 세부로 쪼개거나 결정해야함
	class StaticMeshObject;
	class SkinnedMeshObject;
	class TerrainMeshObject;
	class NodeHierarchy;
	class NodeHierarchyInstance;

	struct StaticMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<StaticMesh> StaticMesh;
		std::shared_ptr<Material> Material;
		StaticMeshObject* StaticMeshObject;
		std::shared_ptr<NodeHierarchyInstance> NodeHierarchyInstnace;
	};

	struct SkinnedMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<SkinnedMesh> SkinnedMesh;
		std::shared_ptr<Material> Material;
		SkinnedMeshObject* SkinnedMeshObject;
		std::shared_ptr<NodeHierarchy> NodeHierarchy;
		std::shared_ptr<NodeHierarchyInstance> NodeHierarchyInstnace;
	};

	class JobHelper
	{
	public:
		static bool CompareStaticMeshJob(const StaticMeshJob& lhs, const StaticMeshJob& rhs);
		static bool CompareSkinnedMeshJob(const SkinnedMeshJob& lhs, const SkinnedMeshJob& rhs);
	};

	struct TerrainMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<TerrainMesh> TerrainMesh;
		std::shared_ptr<TerrainMaterial> TerrainMaterial;
		TerrainMeshObject* TerrainMeshObject;
	};
}