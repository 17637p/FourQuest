#include "D3D11ModelManager.h"
#include "Mesh.h"
#include "Material.h"
#include "BoneHierarchy.h"
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

	void D3D11ModelManager::ConvertModel(const std::string& fbxFile, const std::string& fileName)
	{
		fq::loader::ModelConverter converter;
		converter.ReadFBXFile(fbxFile);
		fq::loader::ModelLoader::Write(converter.Convert(), fileName);
	}

	const fq::common::Model& D3D11ModelManager::CreateModel(const std::shared_ptr<D3D11Device>& device, std::string fileName, std::filesystem::path textureBasePath)
	{
		auto find = mModels.find(fileName);

		if (find != mModels.end())
		{
			return find->second;
		}

		fq::common::Model model = fq::loader::ModelLoader::Read(fileName);

		for (const auto& material : model.Materials)
		{
			if (material.Name.empty())
			{
				continue;
			}

			CreateMaterial(device, GenerateMaterialKey(fileName, material.Name), material, textureBasePath);
		}

		bool bHasSkinnedMesh = false;

		for (const auto& [node, mesh] : model.Meshes)
		{
			if (mesh.Vertices.empty())
			{
				continue;
			}

			if (mesh.BoneVertices.empty())
			{
				CreateStaticMesh(device, GenerateStaticMeshKey(fileName, mesh.Name), mesh);
			}
			else
			{
				CreateSkinnedMesh(device, GenerateSkinnedMeshKey(fileName, mesh.Name), mesh);
				bHasSkinnedMesh = true;
			}
		}

		if (bHasSkinnedMesh)
		{
			CreateBoneHierarchy(GenerateBoneHierarachyKey(fileName), model);
		}

		for (const auto& animation : model.Animations)
		{
			CreateAnimation(GenerateAnimationKey(fileName, animation.Name), animation);
		}

		mModels.insert({ fileName, std::move(model) });

		return mModels[fileName];
	}

	void D3D11ModelManager::DeleteModel(const std::string& path)
	{
		auto find = mModels.find(path);

		if (find == mModels.end())
		{
			return;
		}

		const fq::common::Model& model = find->second;

		for (const auto& material : model.Materials)
		{
			if (material.Name.empty())
			{
				continue;
			}

			DeleteMaterial(path + material.Name);
		}

		bool bHasSkinnedMesh = false;

		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& mesh = nodeMeshPair.second;

			if (mesh.Name.empty())
			{
				continue;
			}

			if (mesh.BoneVertices.empty())
			{
				DeleteStaticMesh(path + mesh.Name);
			}
			else
			{
				DeleteSkinnedMesh(path + mesh.Name);
				bHasSkinnedMesh = true;
			}
		}

		if (bHasSkinnedMesh)
		{
			DeleteBoneHierarchy(path);
		}

		for (const auto& animation : model.Animations)
		{
			DeleteAnimation(path + animation.Name);
		}

		mModels.erase(find);
	}

	bool D3D11ModelManager::CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		return CreateResource(mStaticMeshes, key, device, meshData);
	}
	bool D3D11ModelManager::CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		return CreateResource(mSkinnedMeshes, key, device, meshData);
	}
	bool D3D11ModelManager::CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath)
	{
		MaterialInfo materialInfo;

		materialInfo.BaseColor = matrialData.BaseColor;
		materialInfo.Metalness = matrialData.Metalness;
		materialInfo.Roughness = matrialData.Roughness;

		if (matrialData.BaseColorFileName != L"")materialInfo.BaseColorFileName = basePath / matrialData.BaseColorFileName;
		if (matrialData.MetalnessFileName != L"")materialInfo.MetalnessFileName = basePath / matrialData.MetalnessFileName;
		if (matrialData.RoughnessFileName != L"")materialInfo.RoughnessFileName = basePath / matrialData.RoughnessFileName;
		if (matrialData.NormalFileName != L"")materialInfo.NormalFileName = basePath / matrialData.NormalFileName;
		if (matrialData.EmissiveFileName != L"")materialInfo.EmissiveFileName = basePath / matrialData.EmissiveFileName;

		return CreateResource(mMaterials, key, mResourceManager, materialInfo);
	}
	bool D3D11ModelManager::CreateBoneHierarchy(std::string key, const fq::common::Model modelData)
	{
		return CreateResource(mBoneHierarchies, key, modelData);
	}
	bool D3D11ModelManager::CreateAnimation(std::string key, const fq::common::AnimationClip& animationData)
	{
		return CreateResource(mAnimationClips, key, animationData);
	}

	void D3D11ModelManager::DeleteStaticMesh(const std::string& key)
	{
		mStaticMeshes.erase(key);
	}
	void D3D11ModelManager::DeleteSkinnedMesh(const std::string& key)
	{
		mSkinnedMeshes.erase(key);
	}
	void D3D11ModelManager::DeleteMaterial(const std::string& key)
	{
		mMaterials.erase(key);
	}
	void D3D11ModelManager::DeleteBoneHierarchy(const std::string& key)
	{
		mBoneHierarchies.erase(key);
	}
	void D3D11ModelManager::DeleteAnimation(const std::string& key)
	{
		mAnimationClips.erase(key);
	}

	std::shared_ptr<IParticleMaterial> D3D11ModelManager::CreateMaterial(const ParticleMaterialInfo& materialInfo)
	{
		std::shared_ptr<IParticleMaterial> material = std::make_shared<ParticleMaterial>(mResourceManager, materialInfo);
		mParticleMaterials.insert(material);

		return material;
	}
	void  D3D11ModelManager::DeleteMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial)
	{
		mParticleMaterials.erase(iParticleMaterial);
	}

	std::shared_ptr<IDecalMaterial> D3D11ModelManager::CreateMaterial(const DecalMaterialInfo& materialInfo)
	{
		std::shared_ptr<IDecalMaterial> material = std::make_shared<DecalMaterial>(mResourceManager, materialInfo);
		mDecalMaterials.insert(material);

		return material;
	}
	void  D3D11ModelManager::DeleteMaterial(std::shared_ptr<IDecalMaterial> iDecalMaterial)
	{
		mDecalMaterials.erase(iDecalMaterial);
	}

	const fq::common::Model& D3D11ModelManager::FindModel(const std::string& path)
	{
		auto find = mModels.find(path);
		assert(find != mModels.end());
		return find->second;
	}

	std::shared_ptr<StaticMesh> D3D11ModelManager::FindStaticMeshOrNull(const std::string& key)
	{
		return FindResourceOrNull(mStaticMeshes, key);
	}
	std::shared_ptr<SkinnedMesh> D3D11ModelManager::FindSkinnedMeshOrNull(const std::string& key)
	{
		return FindResourceOrNull(mSkinnedMeshes, key);
	}
	std::shared_ptr<Material> D3D11ModelManager::FindMaterialOrNull(const std::string& key)
	{
		return FindResourceOrNull(mMaterials, key);
	}
	std::shared_ptr<BoneHierarchy> D3D11ModelManager::FindBoneHierarchyOrNull(const std::string& key)
	{
		return FindResourceOrNull(mBoneHierarchies, key);
	}
	std::shared_ptr<fq::common::AnimationClip> D3D11ModelManager::FindAnimationOrNull(const std::string& key)
	{
		return FindResourceOrNull(mAnimationClips, key);
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

	std::vector<std::shared_ptr<IMaterial>> D3D11ModelManager::GetMaterials() const
	{
		std::vector<std::shared_ptr<IMaterial>> materials;

		for (auto& [key, material] : mMaterials)
		{
			materials.push_back(material);
		}

		return materials;
	}
}