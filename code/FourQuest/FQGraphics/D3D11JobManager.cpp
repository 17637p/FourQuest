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
		mSkinnedMeshJobs.reserve(RESERVE_SIZE);
		mTerrainMeshJobs.reserve(RESERVE_SIZE);
	}

	void D3D11JobManager::CreateStaticMeshJob(std::shared_ptr<IStaticMeshObject> iStaticMeshObject)
	{
		const auto& material = iStaticMeshObject->GetMaterials();
		const size_t JOB_COUNT = std::min<size_t>(material.size(), iStaticMeshObject->GetStaticMesh()->GetMeshData().Subsets.size());

		for (size_t i = 0; i < JOB_COUNT; ++i)
		{
			StaticMeshJob job;
			job.SubsetIndex = i;
			job.StaticMesh = std::static_pointer_cast<StaticMesh>(iStaticMeshObject->GetStaticMesh());
			job.Material = std::static_pointer_cast<Material>(material[i]);
			job.StaticMeshObject = std::static_pointer_cast<StaticMeshObject>(iStaticMeshObject);
		}
	}
	void D3D11JobManager::CreateSkinnedMeshJob(std::shared_ptr<ISkinnedMeshObject> iSkinnedMeshObject)
	{
		const auto& material = iSkinnedMeshObject->GetMaterials();
		const size_t JOB_COUNT = std::min<size_t>(material.size(), iSkinnedMeshObject->GetSkinnedMesh()->GetMeshData().Subsets.size());

		for (size_t i = 0; i < JOB_COUNT; ++i)
		{
			SkinnedMeshJob job;
			job.SubsetIndex = i;
			job.SkinnedMesh = std::static_pointer_cast<SkinnedMesh>(iSkinnedMeshObject->GetSkinnedMesh());
			job.Material = std::static_pointer_cast<Material>(material[i]);
			job.SkinnedMeshObject = std::static_pointer_cast<SkinnedMeshObject>(iSkinnedMeshObject);
		}
	}
	void D3D11JobManager::CreateTerrainMeshJob(std::shared_ptr<ITerrainMeshObject> iTerrainMeshObjct)
	{
		std::shared_ptr<TerrainMeshObject> terrainMeshObject = std::static_pointer_cast<TerrainMeshObject>(iTerrainMeshObjct);
		const std::shared_ptr<TerrainMesh>& terrainMesh = terrainMeshObject->GetTerrainMesh();
		const std::shared_ptr<TerrainMaterial> material = terrainMeshObject->GetTerrainMaterial();
		const fq::common::Mesh& meshData = terrainMesh->GetMeshData();

		for (size_t i = 0; i < meshData.Subsets.size(); ++i)
		{
			TerrainMeshJob job;
			job.SubsetIndex = i;
			job.TerrainMaterial = material;
			job.TerrainMesh = terrainMesh;
			job.TerrainMeshObject = terrainMeshObject;
		}
	}

	void D3D11JobManager::ClearAll()
	{
		ClearStaticMeshJobs();
		ClearSkinnedMeshJobs();
		ClearTerrainMeshJobs();
	}
	void D3D11JobManager::ClearStaticMeshJobs()
	{
		mStaticMeshJobs.clear();
	}
	void D3D11JobManager::ClearSkinnedMeshJobs()
	{
		mSkinnedMeshJobs.clear();
	}
	void D3D11JobManager::ClearTerrainMeshJobs()
	{
		mTerrainMeshJobs.clear();
	}
}

