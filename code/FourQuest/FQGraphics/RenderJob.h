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
		StaticMeshObject* StaticMeshObject;
		std::shared_ptr<NodeHierarchyInstance> NodeHierarchyInstnace;
	};

	struct SkinnedMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<SkinnedMesh> SkinnedMesh;
		std::shared_ptr<Material> Material;
		SkinnedMeshObject* SkinnedMeshObject;
		std::shared_ptr<NodeHierarchyInstance> NodeHierarchyInstnace;
	};

	class JobHelper
	{
	public:
		static bool CompareStaticMeshJob(const StaticMeshJob& lhs, const StaticMeshJob& rhs)
		{
			// StaticMesh �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.StaticMesh < rhs.StaticMesh) return true;
			if (rhs.StaticMesh < lhs.StaticMesh) return false;

			// Material �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.Material < rhs.Material) return true;
			if (rhs.Material < lhs.Material) return false;

			// StaticMeshObject �� (������ ��)
			if (lhs.StaticMeshObject < rhs.StaticMeshObject) return true;
			if (rhs.StaticMeshObject < lhs.StaticMeshObject) return false;

			// NodeHierarchyInstance �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.NodeHierarchyInstnace < rhs.NodeHierarchyInstnace) return true;
			if (rhs.NodeHierarchyInstnace < lhs.NodeHierarchyInstnace) return false;

			// SubsetIndex ��
			if (lhs.SubsetIndex < rhs.SubsetIndex) return true;
			if (rhs.SubsetIndex < lhs.SubsetIndex) return false;

			return false;
		}

		static bool CompareSkinnedMeshJob(const SkinnedMeshJob& lhs, const SkinnedMeshJob& rhs)
		{
			// StaticMesh �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.SkinnedMesh < rhs.SkinnedMesh) return true;
			if (rhs.SkinnedMesh < lhs.SkinnedMesh) return false;

			// Material �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.Material < rhs.Material) return true;
			if (rhs.Material < lhs.Material) return false;

			// StaticMeshObject �� (������ ��)
			if (lhs.SkinnedMeshObject < rhs.SkinnedMeshObject) return true;
			if (rhs.SkinnedMeshObject < lhs.SkinnedMeshObject) return false;

			// NodeHierarchyInstance �� (shared_ptr�̹Ƿ� ���� �� ��)
			if (lhs.NodeHierarchyInstnace < rhs.NodeHierarchyInstnace) return true;
			if (rhs.NodeHierarchyInstnace < lhs.NodeHierarchyInstnace) return false;

			// SubsetIndex ��
			if (lhs.SubsetIndex < rhs.SubsetIndex) return true;
			if (rhs.SubsetIndex < lhs.SubsetIndex) return false;

			return false;
		}
	};

	struct TerrainMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<TerrainMesh> TerrainMesh;
		std::shared_ptr<TerrainMaterial> TerrainMaterial;
		TerrainMeshObject* TerrainMeshObject;
	};
}