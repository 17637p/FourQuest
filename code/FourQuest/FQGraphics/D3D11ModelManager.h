#pragma once

#include "CommonHeader.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"
#include "../FQCommon/IFQRenderResource.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class D3D11Device;
	class StaticMesh;
	class SkinnedMesh;
	class Material;
	class NodeHierarchy;
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

		void WriteModel(const std::string& path, const fq::common::Model& modelData);
		fq::common::Model ReadModel(const std::string& path);
		fq::common::Model ConvertModel(const std::string& fbxFilePath);

		const fq::common::Model& CreateModelResource(const std::shared_ptr<D3D11Device>& device, unsigned int key, const std::string& path, std::filesystem::path textureBasePath = "");
		void CreateModelResource(const std::shared_ptr<D3D11Device>& device, unsigned int key, const fq::common::Model& modelData, std::filesystem::path textureBasePath = "");
		bool TryCreateModelResource(const std::shared_ptr<D3D11Device>& device, unsigned int key, const std::string& path, std::filesystem::path textureBasePath = "", fq::common::Model* outDataOrNull = nullptr);
		const fq::common::Model& GetModel(unsigned int key);
		bool TryGetModel(unsigned int key, fq::common::Model* model);
		void DeleteModelResource(unsigned int key);

		std::shared_ptr<INodeHierarchy> GetNodeHierarchyByModelPathOrNull(unsigned int key);
		std::shared_ptr<IStaticMesh> GetStaticMeshByModelPathOrNull(unsigned int key, std::string meshName);
		std::shared_ptr<ISkinnedMesh> GetSkinnedMeshByModelPathOrNull(unsigned int key, std::string meshName);
		std::shared_ptr<IMaterial> GetMaterialByModelPathOrNull(unsigned int key, std::string materialName);
		std::shared_ptr<IAnimation> GetAnimationByModelPathOrNull(unsigned int key, std::string animationName);

		std::shared_ptr<IStaticMesh> CreateStaticMesh(const fq::common::Mesh& meshData);
		std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(const fq::common::Mesh& meshData);
		std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const fq::common::Model& model);
		std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(const std::vector<fq::common::Node> nodes);
		std::shared_ptr<IAnimation> CreateAnimation(const fq::common::AnimationClip& animationClip);
		std::shared_ptr<IUVAnimation> CreateUVAnimation(const fq::common::UVAnimationClip& animationClip);
		std::shared_ptr<IMaterial> CreateMaterial(const MaterialInfo& materialInfo);
		std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const ParticleMaterialInfo& materialInfo);
		std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const DecalMaterialInfo& decalMaterialInfo);

		std::shared_ptr<IStaticMesh> CreateStaticMesh(std::string key, const fq::common::Mesh& meshData);
		std::shared_ptr<ISkinnedMesh> CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData);
		std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const fq::common::Model& model);
		std::shared_ptr<INodeHierarchy> CreateNodeHierarchy(std::string key, const std::vector<fq::common::Node> nodes);
		std::shared_ptr<IAnimation> CreateAnimation(std::string key, const fq::common::AnimationClip& animationClip);
		std::shared_ptr<IUVAnimation> CreateUVAnimation(std::string key, const fq::common::UVAnimationClip& animationClip);
		std::shared_ptr<IMaterial> CreateMaterial(const std::string& key, const MaterialInfo& materialInfo);
		std::shared_ptr<IParticleMaterial> CreateParticleMaterial(const std::string& key, const ParticleMaterialInfo& materialInfo);
		std::shared_ptr<IDecalMaterial> CreateDecalMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo);

		std::vector<std::shared_ptr<IStaticMesh>> GetStaticMeshes();
		std::vector<std::shared_ptr<ISkinnedMesh>> GetSkinnedMeshes();
		std::vector<std::shared_ptr<INodeHierarchy>> GetNodeHierarchies();
		std::vector<std::shared_ptr<IAnimation>> GetAnimations();
		std::vector<std::shared_ptr<IUVAnimation>> GetUVAnimations();
		std::vector<std::shared_ptr<IMaterial>> GetMaterials();
		std::vector<std::shared_ptr<IParticleMaterial>> GetParticleMaterials();
		std::vector<std::shared_ptr<IDecalMaterial>> GetDecalMaterials();

		std::shared_ptr<IStaticMesh> GetStaticMeshOrNull(std::string key);
		std::shared_ptr<ISkinnedMesh> GetSkinnedMeshOrNull(std::string key);
		std::shared_ptr<INodeHierarchy> GetNodeHierarchyOrNull(std::string key);
		std::shared_ptr<IAnimation> GetAnimationOrNull(std::string key);
		std::shared_ptr<IUVAnimation> GetUVAnimationOrNull(std::string key);
		std::shared_ptr<IMaterial> GetMaterialOrNull(const std::string& key);
		std::shared_ptr<IParticleMaterial> GetParticleMaterialOrNull(const std::string& key);
		std::shared_ptr<IDecalMaterial> GetDecalMaterialOrNull(const std::string& key);

		void DeleteStaticMesh(const std::string& key);
		void DeleteSkinnedMesh(const std::string& key);
		void DeleteNodeHierarchy(const std::string& key);
		void DeleteAnimation(const std::string& key);
		void DeleteUVAnimation(const std::string& key);
		void DeleteMaterial(const std::string& key);
		void DeleteParticleMaterial(const std::string& key);
		void DeleteDecalMaterial(const std::string& key);

		std::string GenerateStaticMeshKey(const std::string& fileName, const std::string& meshName);
		std::string GenerateSkinnedMeshKey(const std::string& fileName, const std::string& meshName);
		std::string GenerateMaterialKey(const std::string& fileName, const std::string& materialName);
		std::string GenerateBoneHierarachyKey(const std::string& fileName);
		std::string GenerateAnimationKey(const std::string& fileName, const std::string& animationName);

	private:
		template <typename T>
		std::shared_ptr<T> findOrNull(std::map<std::string, std::shared_ptr<T>>& container, const std::string& key)
		{
			auto find = container.find(key);

			if (find == container.end())
			{
				return nullptr;
			}

			return find->second;
		}

		template <typename T, typename U, typename... Args>
		std::shared_ptr<T> create(std::map<std::string, std::shared_ptr<T>>& container, const std::string& key, Args... args)
		{
			auto find = container.find(key);

			if (find != container.end())
			{
				return find->second;
			}

			auto result = std::make_shared<U>(args...);
			container.insert({ key, result });

			return result;
		}

		template <typename T>
		std::vector<std::shared_ptr<T>> getElementInVector(std::map<std::string, std::shared_ptr<T>>& container)
		{
			std::vector<std::shared_ptr<T>> result;
			result.reserve(container.size());

			for (auto& [key, data] : container)
			{
				result.push_back(data);
			}

			return result;
		}

	private:
		std::shared_ptr<D3D11Device> mDevice;
		std::shared_ptr<D3D11ResourceManager> mResourceManager;

		std::map<unsigned int, fq::common::Model> mModels;
		std::map<std::string, std::shared_ptr<IStaticMesh>> mStaticMeshes;
		std::map<std::string, std::shared_ptr<ISkinnedMesh>> mSkinnedMeshes;
		std::map<std::string, std::shared_ptr<INodeHierarchy>> mNodeHierarchies;
		std::map<std::string, std::shared_ptr<IAnimation>> mAnimations;
		std::map<std::string, std::shared_ptr<IUVAnimation>> mUVAnimations;
		std::map<std::string, std::shared_ptr<IMaterial>> mMaterials;
		std::map<std::string, std::shared_ptr<IParticleMaterial>> mParticleMaterials;
		std::map<std::string, std::shared_ptr<IDecalMaterial>> mDecalMaterials;
	};
}