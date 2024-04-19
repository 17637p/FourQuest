#pragma once

#include <memory>
#include <vector>

#include <directxtk/SimpleMath.h>

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;

	struct StaticMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<StaticMesh> StaticMesh;
		std::shared_ptr<Material> Material;
		const DirectX::SimpleMath::Matrix* TransformPtr; 
	};

	struct SkinnedMeshJob
	{
		size_t SubsetIndex;
		std::shared_ptr<SkinnedMesh> SkinnedMesh;
		std::shared_ptr<Material> Material;
		const DirectX::SimpleMath::Matrix* TransformPtr;
		std::vector<DirectX::SimpleMath::Matrix>* BoneMatricesPtr;
	};
}