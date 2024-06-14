#pragma once

#include <set>
#include <memory>
#include <directxtk\SimpleMath.h>
#include <filesystem>
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class Material;
	class D3D11Device;
	class D3D11ResourceManager;
	class D3D11ModelManager;

	class DecalObject : public IDecalObject
	{
	public:
		DecalObject(const DecalInfo& decalInfo);
		virtual ~DecalObject() = default;

		virtual void SetDecalInfo(const DecalInfo& decalInfo) override;
		virtual const DecalInfo& GetDecalInfo() const override { return mDecalInfo; }

	private:
		DecalInfo mDecalInfo;
	};

	struct DecalJob
	{
		IDecalObject* DecalObjectInterface;
		IMaterial* MaterialInterface;
		const DirectX::SimpleMath::Matrix* Transform;
	};

	class D3D11DecalManager
	{
	public:
		void Initialize(const std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		void PushJob(const DecalJob& job)
		{
			mDecalJobs.push_back(job);
		}
		void ClearJob()
		{
			mDecalJobs.clear();
		}

		IDecalObject* CreateDecalObject(const DecalInfo& decalInfo);
		void DeleteDecalObject(IDecalObject* decalObject);

		const std::set<IDecalObject*> GetDecalObjects();

		const auto& GetJobs() { return mDecalJobs; }

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;
		std::set<IDecalObject*> mDecalObjects;

		std::vector<DecalJob> mDecalJobs;
	};
}
