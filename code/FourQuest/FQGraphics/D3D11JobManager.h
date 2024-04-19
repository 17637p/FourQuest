#pragma once

#include <vector>

#include "RenderJob.h"

namespace fq::graphics
{
	class IStaticMeshObject;

	class D3D11JobManager
	{
	public:
		D3D11JobManager();
		~D3D11JobManager() = default;

		void CreateStaticMeshJob(IStaticMeshObject* iStaticMeshObject);
		void ClearStaticMeshJobs();

		inline const std::vector<StaticMeshJob>& GetStaticMeshJobs() const;

	private:
		enum { RESERVE_SIZE = 2048 };

		std::vector<StaticMeshJob> mStaticMeshJobs;
	};

	inline const std::vector<StaticMeshJob>& D3D11JobManager::GetStaticMeshJobs() const
	{
		return mStaticMeshJobs;
	}
}