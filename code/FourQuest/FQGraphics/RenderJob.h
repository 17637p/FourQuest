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

	// job�� �Ϲ�ȭ��Ű�ų� �� ���η� �ɰ��ų� �����ؾ���
	class StaticMeshObject;
	class SkinnedMeshObject;
	class TerrainMeshObject;
	class NodeHierarchyInstance;

	struct StaticMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<StaticMesh> StaticMesh;
		std::shared_ptr<Material> Material;
		std::shared_ptr<StaticMeshObject> StaticMeshObject;
	};

	struct SkinnedMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<SkinnedMesh> SkinnedMesh;
		std::shared_ptr<Material> Material;
		std::shared_ptr<SkinnedMeshObject> SkinnedMeshObject;
		std::shared_ptr<NodeHierarchyInstance> NodeHierarchyInstnace;
	};

	struct TerrainMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<TerrainMesh> TerrainMesh;
		std::shared_ptr<TerrainMaterial> TerrainMaterial;
		std::shared_ptr<ITerrainMeshObject> TerrainMeshObject;
	};
}