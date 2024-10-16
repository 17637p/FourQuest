#pragma once

#include <vector>
#include <set>
#include "RenderJob.h"

namespace fq::graphics
{
	class IStaticMeshObject;
	class ISkinnedMeshObject;
	class ITerrainMeshObject;

	class D3D11JobManager
	{
	public:
		D3D11JobManager();
		~D3D11JobManager() = default;

		void CreateStaticMeshJob(IStaticMeshObject* iStaticMeshObject);
		void CreateSkinnedMeshJob(ISkinnedMeshObject* iSkinnedMeshObject);
		void CreateTerrainMeshJob(ITerrainMeshObject* iTerrainMeshObjct);

		void ClearAll();
		void ClearStaticMeshJobs();
		void ClearSkinnedMeshJobs();
		void ClearTerrainMeshJobs();

		inline const std::vector<StaticMeshJob>& GetStaticMeshJobs() const;
		inline const std::vector<SkinnedMeshJob>& GetSkinnedMeshJobs() const;
		inline const std::vector<TerrainMeshJob>& GetTerrainMeshJobs() const;

		void SortStaticMeshJob();
		void SortSkinnedMeshJob();

	private:
		enum { RESERVE_SIZE = 2048 };

		std::vector<StaticMeshJob> mStaticMeshJobs;
		std::vector<SkinnedMeshJob> mSkinnedMeshJobs;
		std::vector<TerrainMeshJob> mTerrainMeshJobs;
	};

	inline const std::vector<StaticMeshJob>& D3D11JobManager::GetStaticMeshJobs() const
	{
		return mStaticMeshJobs;
	}
	inline const std::vector<SkinnedMeshJob>& D3D11JobManager::GetSkinnedMeshJobs() const
	{
		return mSkinnedMeshJobs;
	}
	inline const std::vector<TerrainMeshJob>& D3D11JobManager::GetTerrainMeshJobs() const
	{
		return mTerrainMeshJobs;
	}
}