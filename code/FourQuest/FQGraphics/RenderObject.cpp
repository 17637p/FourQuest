#include "RenderObject.h"
#include "BoneHierarchy.h"

namespace fq::graphics
{
	StaticMeshObject::StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform)
		: mStaticMesh(staticMesh)
		, mMaterials(materials)
		, mTransform(transform)
		, mObjectRenderType(EObjectRenderType::Opaque)
		, mAlpha(1.f)
		, mbUseShadow(true)
	{
	}
	SkinnedMeshObject::SkinnedMeshObject(std::shared_ptr<SkinnedMesh> skinnedMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform,
		BoneHierarchyCache boneHierarchyCache)
		: mSkinnedMesh(skinnedMesh)
		, mMaterials(materials)
		, mTransform(transform)
		, mBoneHierarchyCache(boneHierarchyCache)
		, mObjectRenderType(EObjectRenderType::Opaque)
		, mAlpha(1.f)
		, mbUseShadow(true)
	{
	}
}