#pragma once

#include <memory>
#include <vector>

#include <directxtk/SimpleMath.h>

#include "../FQCommon/EObjectRenderType.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;

	// job�� �Ϲ�ȭ��Ű�ų� �� ���η� �ɰ��ų� �����ؾ���
	class IStaticMeshObject;
	class ISkinnedMeshObject;

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
}