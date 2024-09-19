#include "RenderJob.h"
#include "RenderObject.h"

namespace fq::graphics
{
	bool JobHelper::CompareStaticMeshJob(const StaticMeshJob& lhs, const StaticMeshJob& rhs)
	{
		// 오브젝트 타입 비교
		if (lhs.StaticMeshObject->GetMeshObjectInfo().ObjectType < rhs.StaticMeshObject->GetMeshObjectInfo().ObjectType) return true;
		if (rhs.StaticMeshObject->GetMeshObjectInfo().ObjectType < lhs.StaticMeshObject->GetMeshObjectInfo().ObjectType) return false;

		// StaticMesh 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.StaticMesh < rhs.StaticMesh) return true;
		if (rhs.StaticMesh < lhs.StaticMesh) return false;

		// Material 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.Material < rhs.Material) return true;
		if (rhs.Material < lhs.Material) return false;

		// NodeHierarchyInstance 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.NodeHierarchyInstnace < rhs.NodeHierarchyInstnace) return true;
		if (rhs.NodeHierarchyInstnace < lhs.NodeHierarchyInstnace) return false;

		// SubsetIndex 비교
		if (lhs.SubsetIndex < rhs.SubsetIndex) return true;
		if (rhs.SubsetIndex < lhs.SubsetIndex) return false;

		return false;
	}

	bool JobHelper::CompareSkinnedMeshJob(const SkinnedMeshJob& lhs, const SkinnedMeshJob& rhs)
	{
		// StaticMesh 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.SkinnedMesh < rhs.SkinnedMesh) return true;
		if (rhs.SkinnedMesh < lhs.SkinnedMesh) return false;

		// Material 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.Material < rhs.Material) return true;
		if (rhs.Material < lhs.Material) return false;

		// StaticMeshObject 비교 (포인터 비교)
		if (lhs.SkinnedMeshObject < rhs.SkinnedMeshObject) return true;
		if (rhs.SkinnedMeshObject < lhs.SkinnedMeshObject) return false;

		// NodeHierarchyInstance 비교 (shared_ptr이므로 내부 값 비교)
		if (lhs.NodeHierarchyInstnace < rhs.NodeHierarchyInstnace) return true;
		if (rhs.NodeHierarchyInstnace < lhs.NodeHierarchyInstnace) return false;

		// SubsetIndex 비교
		if (lhs.SubsetIndex < rhs.SubsetIndex) return true;
		if (rhs.SubsetIndex < lhs.SubsetIndex) return false;

		return false;
	}
}