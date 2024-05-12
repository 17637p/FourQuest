#pragma once

#include <vector>
#include <set>
#include "RenderJob.h"

namespace fq::graphics
{
	class IStaticMeshObject;
	class ISkinnedMeshObject;

	class D3D11JobManager
	{
	public:
		D3D11JobManager();
		~D3D11JobManager() = default;
		
		void CreateStaticMeshJob(IStaticMeshObject* iStaticMeshObject);
		void CreateStaticMeshJobs(const std::set<IStaticMeshObject*>& staticMeshObjects);
		void CreateSkinnedMeshJob(ISkinnedMeshObject* iSkinnedMeshObject);
		void CreateSkinnedMeshJobs(const std::set<ISkinnedMeshObject*>& skinnedMeshObjects);

		void ClearAll();
		void ClearStaticMeshJobs();
		void ClearSkinnedMeshJobs();

		inline const std::vector<StaticMeshJob>& GetStaticMeshJobs() const;
		inline const std::vector<SkinnedMeshJob>& GetSkinnedMeshJobs() const;

	private:
		enum { RESERVE_SIZE = 2048 };

		std::vector<StaticMeshJob> mStaticMeshJobs;
		std::vector<SkinnedMeshJob> mSkinnedMeshJobs;
		std::vector<SkinnedMeshJob> mTerrainMeshJobs;
	};

	inline const std::vector<StaticMeshJob>& D3D11JobManager::GetStaticMeshJobs() const
	{
		return mStaticMeshJobs;
	}

	inline const std::vector<SkinnedMeshJob>& D3D11JobManager::GetSkinnedMeshJobs() const
	{
		return mSkinnedMeshJobs;
	}
}