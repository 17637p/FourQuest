#include "D3D11Device.h"
#include "D3D11ObjectManager.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"
#include "BoneHierarchy.h"

#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

namespace fq::graphics
{
	void D3D11ObjectManager::ConvertModel(std::string fbxFile, std::string fileName)
	{
		fq::loader::ModelConverter converter;
		converter.ReadFBXFile(fbxFile);
		fq::loader::ModelLoader::Write(converter.Convert(), fileName);
	}
	const fq::common::Model& D3D11ObjectManager::CreateModel(const std::shared_ptr<D3D11Device>& device, std::string fileName, std::filesystem::path textureBasePath)
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

			CreateMaterial(device, fileName + material.Name, material, textureBasePath);
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
				CreateStaticMesh(device, fileName + mesh.Name, mesh);
			}
			else
			{
				CreateSkinnedMesh(device, fileName + mesh.Name, mesh);
				bHasSkinnedMesh = true;
			}
		}

		if (bHasSkinnedMesh)
		{
			CreateBoneHierarchy(fileName, model);
		}

		for (const auto& animation : model.Animations)
		{
			CreateAnimation(fileName + animation.Name, animation);
		}

		mModels.insert({ fileName, std::move(model) });

		return mModels[fileName];
	}

	void D3D11ObjectManager::DeleteModel(std::string path)
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

	const fq::common::Model& D3D11ObjectManager::GetModel(std::string path)
	{
		auto find = mModels.find(path);
		assert(find != mModels.end());
		return find->second;
	}

	bool D3D11ObjectManager::CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		return CreateResource(mStaticMeshResources, key, device, meshData);
	}
	bool D3D11ObjectManager::CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		return CreateResource(mSkinnedMeshResources, key, device, meshData);
	}
	bool D3D11ObjectManager::CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath)
	{
		return CreateResource(mMaterialResources, key, device, matrialData, basePath);
	}
	bool D3D11ObjectManager::CreateBoneHierarchy(std::string key, const fq::common::Model modelData)
	{
		return CreateResource(mBoneHierarchyResources, key, modelData);
	}
	bool D3D11ObjectManager::CreateAnimation(std::string key, const fq::common::AnimationClip& animationData)
	{
		return CreateResource(mAnimationClipResources, key, animationData);
	}

	void D3D11ObjectManager::DeleteStaticMesh(std::string key)
	{
		mStaticMeshResources.erase(key);
	}
	void D3D11ObjectManager::DeleteSkinnedMesh(std::string key)
	{
		mSkinnedMeshResources.erase(key);
	}
	void D3D11ObjectManager::DeleteMaterial(std::string key)
	{
		mMaterialResources.erase(key);
	}
	void D3D11ObjectManager::DeleteBoneHierarchy(std::string key)
	{
		mBoneHierarchyResources.erase(key);
	}
	void D3D11ObjectManager::DeleteAnimation(std::string key)
	{
		mAnimationClipResources.erase(key);
	}

	std::shared_ptr<fq::common::AnimationClip> D3D11ObjectManager::FindAnimationResource(const std::string& key)
	{
		auto find = mAnimationClipResources.find(key);

		if (find != mAnimationClipResources.end())
		{
			return find->second;
		}

		return nullptr;
	}

	void D3D11ObjectManager::AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info)
	{
		std::shared_ptr<fq::common::AnimationClip> animationResource = FindAnimationResource(info.ModelPath + info.AnimationName);

		if (animationResource != nullptr)
		{
			SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(iSkinnedMeshObject);
			skinnedMeshObject->AddAnimation(info.AnimationKey, animationResource);
		}
	}

	IStaticMeshObject* D3D11ObjectManager::CreateStaticMeshObject(MeshObjectInfo info)
	{
		auto findedStaticMesh = mStaticMeshResources.find(info.ModelPath + info.MeshName);

		if (findedStaticMesh == mStaticMeshResources.end())
		{
			return nullptr;
		}

		std::vector<std::shared_ptr<Material>> materials;
		findMaterial(info.ModelPath, info.MaterialNames, &materials);

		StaticMeshObject* staticMeshObject = new StaticMeshObject(findedStaticMesh->second, materials, info.Transform);
		mStaticMeshObjects.insert(staticMeshObject);

		return staticMeshObject;
	}
	void D3D11ObjectManager::DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface)
	{
		mStaticMeshObjects.erase(staticMeshObjectInterface);

		StaticMeshObject* staticMeshObject = static_cast<StaticMeshObject*>(staticMeshObjectInterface);
		delete staticMeshObject;
	}

	ISkinnedMeshObject* D3D11ObjectManager::CreateSkinnedMeshObject(MeshObjectInfo info)
	{
		auto findedStaticMesh = mSkinnedMeshResources.find(info.ModelPath + info.MeshName);

		if (findedStaticMesh == mSkinnedMeshResources.end())
		{
			return nullptr;
		}

		std::vector<std::shared_ptr<Material>> materials;
		findMaterial(info.ModelPath, info.MaterialNames, &materials);

		auto findedBoneHierarchy = mBoneHierarchyResources.find(info.ModelPath);
		assert(findedBoneHierarchy != mBoneHierarchyResources.end());
		BoneHierarchyCache boneHierarchyCache(findedBoneHierarchy->second);

		SkinnedMeshObject* skinnedMeshObject = new SkinnedMeshObject(findedStaticMesh->second, materials, info.Transform, std::move(boneHierarchyCache));
		mSkinnedMeshObjects.insert(skinnedMeshObject);

		return skinnedMeshObject;
	}
	void D3D11ObjectManager::DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface)
	{
		mSkinnedMeshObjects.erase(skinnedMeshObjectInterface);

		SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(skinnedMeshObjectInterface);
		delete skinnedMeshObject;
	}

	void D3D11ObjectManager::findMaterial(const std::string& modelPath,
		const std::vector<std::string>& materialNames,
		std::vector<std::shared_ptr<Material>>* outMaterials) const
	{
		outMaterials->reserve(materialNames.size());

		for (const std::string& materialName : materialNames)
		{
			auto findedMaterial = mMaterialResources.find(modelPath + materialName);

			if (findedMaterial != mMaterialResources.end())
			{
				outMaterials->push_back(findedMaterial->second);
			}
			else
			{
				outMaterials->push_back(nullptr);
			}
		}
	}
}