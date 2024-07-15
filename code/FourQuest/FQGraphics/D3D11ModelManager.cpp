#include "D3D11ModelManager.h"
#include "Mesh.h"
#include "Material.h"
#include "NodeHierarchy.h"
#include "Animation.h"
#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

namespace fq::graphics
{
	void D3D11ModelManager::Initialize(std::shared_ptr<D3D11Device> device, std::shared_ptr<D3D11ResourceManager> resourceManager)
	{
		mDevice = device;
		mResourceManager = resourceManager;
	}

	void D3D11ModelManager::WriteModel(const std::string& fileName, const fq::common::Model& modelData)
	{
		fq::loader::ModelLoader::Write(modelData, fileName);
	}

	fq::common::Model D3D11ModelManager::ReadModel(const std::string& path)
	{
		return fq::loader::ModelLoader::Read(path);
	}

	fq::common::Model D3D11ModelManager::ConvertModel(const std::string& fbxFilePath)
	{
		fq::loader::ModelConverter converter;
		converter.ReadFBXFile(fbxFilePath);

		return converter.Convert();
	}

	const fq::common::Model& D3D11ModelManager::CreateModelResource(const std::shared_ptr<D3D11Device>& device, unsigned int key, const std::string& path, std::filesystem::path textureBasePath)
	{
		auto find = mModels.find(key);

		if (find != mModels.end())
		{
			return find->second;
		}

		assert(std::filesystem::exists(path));

		fq::common::Model model = fq::loader::ModelLoader::Read(path);

		for (const auto& material : model.Materials)
		{
			if (material.Name.empty())
			{
				continue;
			}

			MaterialInfo materialInfo;

			materialInfo.BaseColor = material.BaseColor;
			materialInfo.Metalness = material.Metalness;
			materialInfo.Roughness = material.Roughness;

			if (material.BaseColorFileName != L"") materialInfo.BaseColorFileName = textureBasePath / material.BaseColorFileName;
			if (material.MetalnessFileName != L"") materialInfo.MetalnessFileName = textureBasePath / material.MetalnessFileName;
			if (material.RoughnessFileName != L"") materialInfo.RoughnessFileName = textureBasePath / material.RoughnessFileName;
			if (material.NormalFileName != L"") materialInfo.NormalFileName = textureBasePath / material.NormalFileName;
			if (material.EmissiveFileName != L"") materialInfo.EmissiveFileName = textureBasePath / material.EmissiveFileName;
			   
			CreateMaterial(GenerateMaterialKey(std::to_string(key), material.Name), materialInfo);
		}
		    
		for (const auto& [node, mesh] : model.Meshes)
		{    
			if (mesh.Vertices.empty())
			{
				continue;
			}

			if (mesh.BoneVertices.empty())
			{
				CreateStaticMesh(GenerateStaticMeshKey(std::to_string(key), mesh.Name), mesh);
			}
			else
			{
				CreateSkinnedMesh(GenerateSkinnedMeshKey(std::to_string(key), mesh.Name), mesh);
			}
		}

		CreateNodeHierarchy(GenerateBoneHierarachyKey(std::to_string(key)), model);

		for (const auto& animation : model.Animations)
		{
			CreateAnimation(GenerateAnimationKey(std::to_string(key), animation.Name), animation);
		}

		mModels.insert({ key, std::move(model) });

		return mModels[key];
	}

	bool D3D11ModelManager::TryCreateModelResource(const std::shared_ptr<D3D11Device>& device, unsigned int key, const std::string& path, std::filesystem::path textureBasePath, fq::common::Model* outDataOrNull)
	{
		auto find = mModels.find(key);

		if (find != mModels.end())
		{
			if (outDataOrNull != nullptr)
			{
				*outDataOrNull = find->second;
			}

			return true;
		}

		std::filesystem::path fileSystemPath = path;

		if (!std::filesystem::exists(fileSystemPath)
			|| fileSystemPath.extension() != ".model")
		{
			return false;
		}

		fq::common::Model model = fq::loader::ModelLoader::Read(path);

		for (const auto& material : model.Materials)
		{
			if (material.Name.empty())
			{
				continue;
			}

			MaterialInfo materialInfo;

			materialInfo.BaseColor = material.BaseColor;
			materialInfo.Metalness = material.Metalness;
			materialInfo.Roughness = material.Roughness;

			if (material.BaseColorFileName != L"") materialInfo.BaseColorFileName = textureBasePath / material.BaseColorFileName;
			if (material.MetalnessFileName != L"") materialInfo.MetalnessFileName = textureBasePath / material.MetalnessFileName;
			if (material.RoughnessFileName != L"") materialInfo.RoughnessFileName = textureBasePath / material.RoughnessFileName;
			if (material.NormalFileName != L"") materialInfo.NormalFileName = textureBasePath / material.NormalFileName;
			if (material.EmissiveFileName != L"") materialInfo.EmissiveFileName = textureBasePath / material.EmissiveFileName;

			CreateMaterial(GenerateMaterialKey(std::to_string(key), material.Name), materialInfo);
		}

		for (const auto& [node, mesh] : model.Meshes)
		{
			if (mesh.Vertices.empty())
			{
				continue;
			}

			if (mesh.BoneVertices.empty())
			{
				CreateStaticMesh(GenerateStaticMeshKey(std::to_string(key), mesh.Name), mesh);
			}
			else
			{
				CreateSkinnedMesh(GenerateSkinnedMeshKey(std::to_string(key), mesh.Name), mesh);
			}
		}

		CreateNodeHierarchy(GenerateBoneHierarachyKey(std::to_string(key)), model);

		for (const auto& animation : model.Animations)
		{
			CreateAnimation(GenerateAnimationKey(std::to_string(key), animation.Name), animation);
		}

		mModels.insert({ key, std::move(model) });

		if (outDataOrNull != nullptr)
		{
			*outDataOrNull = mModels[key];
		}

		return true;
	}

	const fq::common::Model& D3D11ModelManager::GetModel(unsigned int key)
	{
		auto find = mModels.find(key);
		assert(find != mModels.end());

		return find->second;
	}

	bool D3D11ModelManager::TryGetModel(unsigned int key, fq::common::Model* model)
	{
		auto find = mModels.find(key);

		if (find != mModels.end())
		{
			return false;
		}

		*model = find->second;

		return true;
	}

	void D3D11ModelManager::DeleteModelResource(unsigned int key)
	{
		auto find = mModels.find(key);

		if (find == mModels.end())
		{
			return;
		}

		const fq::common::Model& model = find->second;

		for (const auto& material : model.Materials)
		{
			if (!material.Name.empty())
			{
				DeleteMaterial(GenerateMaterialKey(std::to_string(key), material.Name));
			}
		}

		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& mesh = nodeMeshPair.second;

			if (!mesh.Name.empty())
			{
				if (mesh.BoneVertices.empty())
				{
					DeleteStaticMesh(GenerateStaticMeshKey(std::to_string(key), mesh.Name));
				}
				else
				{
					DeleteSkinnedMesh(GenerateSkinnedMeshKey(std::to_string(key), mesh.Name));
				}
			}
		}

		DeleteNodeHierarchy(std::to_string(key));

		for (const auto& animation : model.Animations)
		{
			DeleteAnimation(GenerateAnimationKey(std::to_string(key), animation.Name));
		}

		mModels.erase(find);
	}

	std::shared_ptr<IStaticMesh> D3D11ModelManager::GetStaticMeshByModelPathOrNull(unsigned int key, std::string meshName)
	{
		return findOrNull(mStaticMeshes, GenerateStaticMeshKey(std::to_string(key), meshName));
	}
	std::shared_ptr<ISkinnedMesh> D3D11ModelManager::GetSkinnedMeshByModelPathOrNull(unsigned int key, std::string meshName)
	{
		return findOrNull(mSkinnedMeshes, GenerateSkinnedMeshKey(std::to_string(key), meshName));
	}
	std::shared_ptr<INodeHierarchy> D3D11ModelManager::GetNodeHierarchyByModelPathOrNull(unsigned int key)
	{
		return findOrNull(mNodeHierarchies, std::to_string(key));
	}
	std::shared_ptr<IMaterial> D3D11ModelManager::GetMaterialByModelPathOrNull(unsigned int key, std::string materialName)
	{
		return findOrNull(mMaterials, GenerateMaterialKey(std::to_string(key), materialName));
	}
	std::shared_ptr<IAnimation> D3D11ModelManager::GetAnimationByModelPathOrNull(unsigned int key, std::string animationName)
	{
		return findOrNull(mAnimations, GenerateAnimationKey(std::to_string(key), animationName));
	}

	std::shared_ptr<IStaticMesh> D3D11ModelManager::CreateStaticMesh(const fq::common::Mesh& meshData)
	{
		std::shared_ptr<IStaticMesh> staticMesh = std::make_shared<StaticMesh>(mDevice, meshData);
		return staticMesh;
	}
	std::shared_ptr<ISkinnedMesh> D3D11ModelManager::CreateSkinnedMesh(const fq::common::Mesh& meshData)
	{
		std::shared_ptr<ISkinnedMesh> skinnedMesh = std::make_shared<SkinnedMesh>(mDevice, meshData);
		return skinnedMesh;
	}
	std::shared_ptr<INodeHierarchy> D3D11ModelManager::CreateNodeHierarchy(const fq::common::Model& model)
	{
		std::shared_ptr<INodeHierarchy> nodeHierarchy = std::make_shared<NodeHierarchy>(model);
		return nodeHierarchy;
	}
	std::shared_ptr<INodeHierarchy> D3D11ModelManager::CreateNodeHierarchy(const std::vector<fq::common::Node> nodes)
	{
		std::shared_ptr<INodeHierarchy> nodeHierarchy = std::make_shared<NodeHierarchy>(nodes);
		return nodeHierarchy;
	}
	std::shared_ptr<IAnimation> D3D11ModelManager::CreateAnimation(const fq::common::AnimationClip& animationClip)
	{
		std::shared_ptr<IAnimation> animation = std::make_shared<Animation>(animationClip);
		return std::shared_ptr<IAnimation>();
	}
	std::shared_ptr<IMaterial> D3D11ModelManager::CreateMaterial(const MaterialInfo& materialInfo)
	{
		std::shared_ptr<IMaterial> material = std::make_shared<Material>(mResourceManager, materialInfo);
		return material;
	}
	std::shared_ptr<IParticleMaterial> D3D11ModelManager::CreateParticleMaterial(const ParticleMaterialInfo& particleMaterialInfo)
	{
		std::shared_ptr<IParticleMaterial> material = std::make_shared<ParticleMaterial>(mResourceManager, particleMaterialInfo);
		return material;
	}
	std::shared_ptr<IDecalMaterial> D3D11ModelManager::CreateDecalMaterial(const DecalMaterialInfo& decalMaterialInfo)
	{
		std::shared_ptr<IDecalMaterial> material = std::make_shared<DecalMaterial>(mResourceManager, decalMaterialInfo);
		return material;
	}

	std::shared_ptr<IStaticMesh> D3D11ModelManager::CreateStaticMesh(std::string key, const fq::common::Mesh& meshData)
	{
		return create<IStaticMesh, StaticMesh>(mStaticMeshes, key, mDevice, meshData);
	}
	std::shared_ptr<ISkinnedMesh> D3D11ModelManager::CreateSkinnedMesh(std::string key, const fq::common::Mesh& meshData)
	{
		return create<ISkinnedMesh, SkinnedMesh>(mSkinnedMeshes, key, mDevice, meshData);
	}
	std::shared_ptr<INodeHierarchy> D3D11ModelManager::CreateNodeHierarchy(std::string key, const fq::common::Model& model)
	{
		return create<INodeHierarchy, NodeHierarchy>(mNodeHierarchies, key, model);
	}
	std::shared_ptr<INodeHierarchy> D3D11ModelManager::CreateNodeHierarchy(std::string key, const std::vector<fq::common::Node> nodes)
	{
		return create<INodeHierarchy, NodeHierarchy>(mNodeHierarchies, key, nodes);
	}
	std::shared_ptr<IAnimation> D3D11ModelManager::CreateAnimation(std::string key, const fq::common::AnimationClip& animationData)
	{
		return create<IAnimation, Animation>(mAnimations, key, animationData);
	}
	std::shared_ptr<IMaterial> D3D11ModelManager::CreateMaterial(const std::string& key, const MaterialInfo& materialInfo)
	{
		return create<IMaterial, Material>(mMaterials, key, mResourceManager, materialInfo);
	}
	std::shared_ptr<IParticleMaterial> D3D11ModelManager::CreateParticleMaterial(const std::string& key, const ParticleMaterialInfo& particleMaterialInfo)
	{
		return create<IParticleMaterial, ParticleMaterial>(mParticleMaterials, key, mResourceManager, particleMaterialInfo);
	}
	std::shared_ptr<IDecalMaterial> D3D11ModelManager::CreateDecalMaterial(const std::string& key, const DecalMaterialInfo& decalMaterialInfo)
	{
		return create<IDecalMaterial, DecalMaterial>(mDecalMaterials, key, mResourceManager, decalMaterialInfo);
	}

	std::vector<std::shared_ptr<IStaticMesh>> D3D11ModelManager::GetStaticMeshes()
	{
		return getElementInVector(mStaticMeshes);
	}
	std::vector<std::shared_ptr<ISkinnedMesh>> D3D11ModelManager::GetSkinnedMeshes()
	{
		return getElementInVector(mSkinnedMeshes);
	}
	std::vector<std::shared_ptr<INodeHierarchy>> D3D11ModelManager::GetNodeHierarchies()
	{
		return getElementInVector(mNodeHierarchies);
	}
	std::vector<std::shared_ptr<IAnimation>> D3D11ModelManager::GetAnimations()
	{
		return getElementInVector(mAnimations);
	}
	std::vector<std::shared_ptr<IMaterial>> D3D11ModelManager::GetMaterials()
	{
		return getElementInVector(mMaterials);
	}
	std::vector<std::shared_ptr<IParticleMaterial>> D3D11ModelManager::GetParticleMaterials()
	{
		return getElementInVector(mParticleMaterials);
	}
	std::vector<std::shared_ptr<IDecalMaterial>> D3D11ModelManager::GetDecalMaterials()
	{
		return getElementInVector(mDecalMaterials);
	}

	std::shared_ptr<IStaticMesh> D3D11ModelManager::GetStaticMeshOrNull(std::string key)
	{
		return findOrNull(mStaticMeshes, key);
	}

	std::shared_ptr<ISkinnedMesh> D3D11ModelManager::GetSkinnedMeshOrNull(std::string key)
	{
		return findOrNull(mSkinnedMeshes, key);
	}

	std::shared_ptr<INodeHierarchy> D3D11ModelManager::GetNodeHierarchyOrNull(std::string key)
	{
		return findOrNull(mNodeHierarchies, key);
	}

	std::shared_ptr<IAnimation> D3D11ModelManager::GetAnimationOrNull(std::string key)
	{
		return findOrNull(mAnimations, key);
	}

	std::shared_ptr<IMaterial> D3D11ModelManager::GetMaterialOrNull(const std::string& key)
	{
		return findOrNull(mMaterials, key);
	}

	std::shared_ptr<IParticleMaterial> D3D11ModelManager::GetParticleMaterialOrNull(const std::string& key)
	{
		return findOrNull(mParticleMaterials, key);
	}

	std::shared_ptr<IDecalMaterial> D3D11ModelManager::GetDecalMaterialOrNull(const std::string& key)
	{
		return findOrNull(mDecalMaterials, key);
	}

	void D3D11ModelManager::DeleteStaticMesh(const std::string& key)
	{
		mStaticMeshes.erase(key);
	}
	void D3D11ModelManager::DeleteSkinnedMesh(const std::string& key)
	{
		mSkinnedMeshes.erase(key);
	}
	void D3D11ModelManager::DeleteNodeHierarchy(const std::string& key)
	{
		mNodeHierarchies.erase(key);
	}
	void D3D11ModelManager::DeleteAnimation(const std::string& key)
	{
		mAnimations.erase(key);
	}
	void D3D11ModelManager::DeleteMaterial(const std::string& key)
	{
		mMaterials.erase(key);
	}
	void D3D11ModelManager::DeleteParticleMaterial(const std::string& key)
	{
		mParticleMaterials.erase(key);
	}
	void D3D11ModelManager::DeleteDecalMaterial(const std::string& key)
	{
		mDecalMaterials.erase(key);
	}

	std::string D3D11ModelManager::GenerateStaticMeshKey(const std::string& fileName, const std::string& meshName)
	{
		return fileName + meshName;
	}
	std::string D3D11ModelManager::GenerateSkinnedMeshKey(const std::string& fileName, const std::string& meshName)
	{
		return fileName + meshName;
	}
	std::string D3D11ModelManager::GenerateMaterialKey(const std::string& fileName, const std::string& materialName)
	{
		return fileName + materialName;
	}
	std::string D3D11ModelManager::GenerateBoneHierarachyKey(const std::string& fileName)
	{
		return fileName;
	}
	std::string D3D11ModelManager::GenerateAnimationKey(const std::string& fileName, const std::string& animationName)
	{
		return fileName + animationName;
	}

}