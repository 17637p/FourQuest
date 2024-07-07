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

		void AddStaticMeshJob(const StaticMeshJob& job)
		{
			mStaticMeshJobs.push_back(job);
		}
		void CreateSkinnedMeshJob(ISkinnedMeshObject* iSkinnedMeshObject);
		void CreateTerrainMeshJob(ITerrainMeshObject* iTerrainMeshObjct);

		void AddInstanceingMeshObject(IStaticMeshObject* iStaticMeshObject);

		void ClearAll();
		void ClearStaticMeshJobs();
		void ClearSkinnedMeshJobs();
		void ClearTerrainMeshJobs();

		inline const std::vector<StaticMeshJob>& GetStaticMeshJobs() const;
		inline const std::vector<SkinnedMeshJob>& GetSkinnedMeshJobs() const;
		inline const std::vector<TerrainMeshJob>& GetTerrainMeshJobs() const;

		const std::set<IStaticMeshObject*> GetInstanceStaticMeshObjects() const { return mInstanceStaticMeshObjects; }

	private:
		enum { RESERVE_SIZE = 2048 };

		std::vector<StaticMeshJob> mStaticMeshJobs;
		std::vector<SkinnedMeshJob> mSkinnedMeshJobs;
		std::vector<TerrainMeshJob> mTerrainMeshJobs;

		std::set<IStaticMeshObject*> mInstanceStaticMeshObjects;
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