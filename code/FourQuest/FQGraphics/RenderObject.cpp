#include "RenderObject.h"

namespace fq::graphics
{
	StaticMeshObject::StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform)
		: mStaticMesh(staticMesh)
		, mMaterials(materials)
		, mTransform(transform)
	{
	}
	void StaticMeshObject::UpdateTransform(const fq::common::Transform& transform)
	{
		mTransform = transform.worldMatrix;
	}

	SkinnedMeshObject::SkinnedMeshObject(std::shared_ptr<SkinnedMesh> skinnedMesh,
		std::vector<std::shared_ptr<Material>> materials,
		DirectX::SimpleMath::Matrix transform)
		: mSkinnedMesh(skinnedMesh)
		, mMaterials(materials)
		, mTransform(transform)
	{
	}
	void SkinnedMeshObject::UpdateTransform(const fq::common::Transform& transform)
	{
		mTransform = transform.worldMatrix;
	}
	void SkinnedMeshObject::UpdateBoneMatrix(const std::vector<DirectX::SimpleMath::Matrix>& boneMatrices)
	{
		mBoneMatrices = boneMatrices;
	}
}