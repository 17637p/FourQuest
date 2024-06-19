#pragma once

#include "CommonHeader.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;
	class StaticMesh;
	class SkinnedMesh;
	class Material;
	class BoneHierarchy;
	class D3D11ResourceManager;
	class IMaterial;
	class IParticleMaterial;
	class IDecalMaterial;

	class D3D11ModelManager
	{
	public:
		D3D11ModelManager() = default;
		~D3D11ModelManager() = default;

		void Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager);

		void WriteModel(const std::string& fileName, const fq::common::Model& modelData);
		void ConvertModel(const std::string& fbxFile, const std::string& fileName);

		const fq::common::Model& CreateModel(const std::shared_ptr<D3D11Device>& device, std::string fileName, std::filesystem::path textureBasePath = "");
		template <typename Resource, typename... Args>
		bool CreateResource(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key, Args... args);
		bool CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath);
		bool CreateBoneHierarchy(std::string key, const fq::common::Model modelData);
		bool CreateAnimation(std::string key, const fq::common::AnimationClip& animationData);

		void DeleteModel(const std::string& path);
		void DeleteStaticMesh(const std::string& key);
		void DeleteSkinnedMesh(const std::string& key);
		void DeleteMaterial(const std::string& key);
		void DeleteBoneHierarchy(const std::string& key);
		void DeleteAnimation(const std::string& key);

		std::shared_ptr<IParticleMaterial> CreateMaterial(const ParticleMaterialInfo& materialInfo);
		void  DeleteMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial);

		std::shared_ptr<IDecalMaterial> CreateMaterial(const DecalMaterialInfo& decalMaterialInfo);
		void  DeleteMaterial(std::shared_ptr<IDecalMaterial> iDecalMaterial);

		template <typename Resource>
		std::shared_ptr<Resource> FindResourceOrNull(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key);
		const fq::common::Model& FindModel(const std::string& path);
		std::shared_ptr<StaticMesh> FindStaticMeshOrNull(const std::string& key);
		std::shared_ptr<SkinnedMesh> FindSkinnedMeshOrNull(const std::string& key);
		std::shared_ptr<Material> FindMaterialOrNull(const std::string& key);
		std::shared_ptr<BoneHierarchy> FindBoneHierarchyOrNull(const std::string& key);
		std::shared_ptr<fq::common::AnimationClip> FindAnimationOrNull(const std::string& key);

		std::string GenerateStaticMeshKey(const std::string& fileName, const std::string& meshName);
		std::string GenerateSkinnedMeshKey(const std::string& fileName, const std::string& meshName);
		std::string GenerateMaterialKey(const std::string& fileName, const std::string& materialName);
		std::string GenerateBoneHierarachyKey(const std::string& fileName);
		std::string GenerateAnimationKey(const std::string& fileName, const std::string& animationName);

		std::vector<std::shared_ptr<IMaterial>> GetMaterials() const;

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::map<std::string, fq::common::Model> mModels;
		std::map<std::string, std::shared_ptr<StaticMesh>> mStaticMeshes;
		std::map<std::string, std::shared_ptr<SkinnedMesh>> mSkinnedMeshes;
		std::map<std::string, std::shared_ptr<Material>> mMaterials;
		std::map<std::string, std::shared_ptr<BoneHierarchy>> mBoneHierarchies;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationClips;

		std::set<std::shared_ptr<IParticleMaterial>> mParticleMaterials;
		std::set<std::shared_ptr<IDecalMaterial>> mDecalMaterials;
	};

	template <typename Resource, typename... Args>
	bool D3D11ModelManager::CreateResource(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key, Args... args)
	{
		auto find = storage.find(key);

		if (find != storage.end())
		{
			return false;
		}

		std::shared_ptr<Resource> resource = std::make_shared<Resource>(args...);
		storage.insert({ key, resource });

		return true;
	}

	template <typename Resource>
	std::shared_ptr<Resource> D3D11ModelManager::FindResourceOrNull(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key)
	{
		auto find = storage.find(key);

		if (find == storage.end())
		{
			return nullptr;
		}

		return find->second;
	}
}