#include "RenderJob.h"
#include "RenderObject.h"

namespace fq::graphics
{
	bool JobHelper::CompareStaticMeshJob(const StaticMeshJob& lhs, const StaticMeshJob& rhs)
	{
		// ������Ʈ Ÿ�� ��
		if (lhs.StaticMeshObject->GetMeshObjectInfo().ObjectType < rhs.StaticMeshObject->GetMeshObjectInfo().ObjectType) return true;
		if (rhs.StaticMeshObject->GetMeshObjectInfo().ObjectType < lhs.StaticMeshObject->GetMeshObjectInfo().ObjectType) return false;

		// StaticMesh �� (shared_ptr�̹Ƿ� ���� �� ��)
		if (lhs.StaticMesh < rhs.StaticMesh) return true;
		if (rhs.StaticMesh < lhs.StaticMesh) return false;

		// Material �� (shared_ptr�̹Ƿ� ���� �� ��)
		if (lhs.Material < rhs.Material) return true;
		if (rhs.Material < lhs.Material) return false;

		// NodeHierarchyInstance �� (shared_ptr�̹Ƿ� ���� �� ��)
		if (lhs.NodeHierarchyInstnace < rhs.NodeHierarchyInstnace) return true;
		if (rhs.NodeHierarchyInstnace < lhs.NodeHierarchyInstnace) return false;

		// SubsetIndex ��
		if (lhs.SubsetIndex < rhs.SubsetIndex) return true;
		if (rhs.SubsetIndex < lhs.SubsetIndex) return false;

		return false;
	}

	bool JobHelper::CompareSkinnedMeshJob(const SkinnedMeshJob& lhs, const SkinnedMeshJob& rhs)
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
}