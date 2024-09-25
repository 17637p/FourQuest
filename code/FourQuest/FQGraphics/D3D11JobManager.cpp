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

	void D3D11JobManager::CreateStaticMeshJob(IStaticMeshObject* iStaticMeshObject)
	{
		if (!iStaticMeshObject->GetIsRender())
		{
			return;
		}

		const auto& material = iStaticMeshObject->GetMaterials();
		const size_t JOB_COUNT = std::min<size_t>(material.size(), iStaticMeshObject->GetStaticMesh()->GetMeshData().Subsets.size());

		for (size_t i = 0; i < JOB_COUNT; ++i)
		{
			StaticMeshJob job;
			job.SubsetIndex = i;
			job.StaticMesh = std::static_pointer_cast<StaticMesh>(iStaticMeshObject->GetStaticMesh());
			job.Material = std::static_pointer_cast<Material>(material[i]);
			job.StaticMeshObject = static_cast<StaticMeshObject*>(iStaticMeshObject);
			job.NodeHierarchyInstnace = std::static_pointer_cast<NodeHierarchyInstance>(iStaticMeshObject->GetNodeHierarchyInstance());

			mStaticMeshJobs.push_back(job);
		}
	}
	void D3D11JobManager::CreateSkinnedMeshJob(ISkinnedMeshObject* iSkinnedMeshObject)
	{
		if (!iSkinnedMeshObject->GetIsRender())
		{
			return;
		}

		const auto& material = iSkinnedMeshObject->GetMaterials();
		const size_t JOB_COUNT = std::min<size_t>(material.size(), iSkinnedMeshObject->GetSkinnedMesh()->GetMeshData().Subsets.size());

		for (size_t i = 0; i < JOB_COUNT; ++i)
		{
			SkinnedMeshJob job{};
			job.SubsetIndex = i;
			job.SkinnedMesh = std::static_pointer_cast<SkinnedMesh>(iSkinnedMeshObject->GetSkinnedMesh());
			job.Material = std::static_pointer_cast<Material>(material[i]);
			job.SkinnedMeshObject = static_cast<SkinnedMeshObject*>(iSkinnedMeshObject);
			job.NodeHierarchyInstnace = std::static_pointer_cast<NodeHierarchyInstance>(iSkinnedMeshObject->GetNodeHierarchyInstance());
			job.NodeHierarchy = job.NodeHierarchyInstnace == nullptr ? nullptr : std::static_pointer_cast<NodeHierarchy>(job.NodeHierarchyInstnace->GetNodeHierarchy());
			mSkinnedMeshJobs.push_back(job);
		}
	}
	void D3D11JobManager::CreateTerrainMeshJob(ITerrainMeshObject* iTerrainMeshObjct)
	{
		TerrainMeshObject* terrainMeshObject = static_cast<TerrainMeshObject*>(iTerrainMeshObjct);
		const std::shared_ptr<TerrainMesh>& terrainMesh = terrainMeshObject->GetTerrainMesh();
		const std::shared_ptr<TerrainMaterial> material = terrainMeshObject->GetTerrainMaterial();
		const fq::common::Mesh& meshData = terrainMesh->GetMeshData();

		for (size_t i = 0; i < meshData.Subsets.size(); ++i)
		{
			TerrainMeshJob job;
			job.SubsetIndex = i;
			job.TerrainMaterial = material;
			job.TerrainMesh = terrainMesh;
			job.TerrainMeshObject = static_cast<TerrainMeshObject*>(terrainMeshObject);

			mTerrainMeshJobs.push_back(job);
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

	void D3D11JobManager::SortStaticMeshJob()
	{
		std::sort(mStaticMeshJobs.begin(), mStaticMeshJobs.end(), [](auto lhs, auto rhs) { return JobHelper::CompareStaticMeshJob(lhs, rhs); });
	}

	void D3D11JobManager::SortSkinnedMeshJob()
	{
		std::sort(mSkinnedMeshJobs.begin(), mSkinnedMeshJobs.end(), [](auto lhs, auto rhs) { return JobHelper::CompareSkinnedMeshJob(lhs, rhs); });
	}
}

