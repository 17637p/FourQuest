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
			job.ObjectRenderType = staticMeshObject->GetObjectRenderType();

			mStaticMeshJobs.push_back(job);
		}
	}
	void D3D11JobManager::CreateStaticMeshJobs(const std::set<IStaticMeshObject*>& staticMeshObjects)
	{
		for (IStaticMeshObject* iStaticMeshObject : staticMeshObjects)
		{
			CreateStaticMeshJob(iStaticMeshObject);
		}
	}
	void D3D11JobManager::CreateSkinnedMeshJob(ISkinnedMeshObject* iSkinnedMeshObject)
	{
		SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(iSkinnedMeshObject);
		const std::shared_ptr<SkinnedMesh>& skinnedMesh = skinnedMeshObject->GetSkinnedMesh();
		const std::vector<std::shared_ptr<Material>> materials = skinnedMeshObject->GetMaterials();
		const fq::common::Mesh& meshData = skinnedMesh->GetMeshData();
		const std::vector<DirectX::SimpleMath::Matrix>& finalTransforms = skinnedMeshObject->GetFinalTransforms();
	
		assert(meshData.Subsets.size() <= materials.size());

		for (size_t i = 0; i < meshData.Subsets.size(); ++i)
		{
			SkinnedMeshJob job;
			job.SubsetIndex = i;
			job.TransformPtr = &skinnedMeshObject->GetTransform();
			job.Material = materials[i];
			job.SkinnedMesh = skinnedMesh;
			job.BoneMatricesPtr = &finalTransforms;
			job.ObjectRenderType = skinnedMeshObject->GetObjectRenderType();

			mSkinnedMeshJobs.push_back(job);
		}
	}
	void D3D11JobManager::CreateSkinnedMeshJobs(const std::set<ISkinnedMeshObject*>& skinnedMeshObjects)
	{
		for (ISkinnedMeshObject* iSkinnedMeshRenderJob : skinnedMeshObjects)
		{
			CreateSkinnedMeshJob(iSkinnedMeshRenderJob);
		}
	}

	void D3D11JobManager::ClearAll()
	{
		ClearStaticMeshJobs();
		ClearSkinnedMeshJobs();
	}
	void D3D11JobManager::ClearStaticMeshJobs()
	{
		mStaticMeshJobs.clear();
	}
	void D3D11JobManager::ClearSkinnedMeshJobs()
	{
		mSkinnedMeshJobs.clear();
	}
}