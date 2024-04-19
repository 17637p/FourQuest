#include "D3D11JobManager.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"
#include "../FQCommon/FQCommonLoader.h"

namespace fq::graphics
{
	D3D11JobManager::D3D11JobManager()
	{
		mStaticMeshJobs.reserve(RESERVE_SIZE);
	}
	void D3D11JobManager::CreateStaticMeshJob(IStaticMeshObject* iStaticMeshObject)
	{
		StaticMeshObject* staticMeshObject = static_cast<StaticMeshObject*>(iStaticMeshObject);
		const std::shared_ptr<StaticMesh>& staticMesh = staticMeshObject->GetStaticMesh();
		const std::vector<std::shared_ptr<Material>> materials = staticMeshObject->GetMaterials();
		const fq::common::Mesh& meshData = staticMesh->GetMeshData();

		assert(meshData.Subsets.size() <= materials.size());
		for (size_t i = 0; i < meshData.Subsets.size(); ++i)
		{
			StaticMeshJob job;
			job.SubsetIndex = i;
			job.TransformPtr = &staticMeshObject->GetTransform();
			job.Material = materials[i];
			job.StaticMesh = staticMesh;

			mStaticMeshJobs.push_back(job);
		}
	}
	void D3D11JobManager::ClearStaticMeshJobs()
	{
		mStaticMeshJobs.clear();
	}
}