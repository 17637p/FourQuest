#include "D3D11Device.h"
#include "D3D11ObjectManager.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"

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

		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& mesh = nodeMeshPair.second;

			if (mesh.Name.empty())
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
			}
		}

		mModels.insert({ fileName, std::move(model) });

		return mModels[fileName];
	}
	const fq::common::Model& D3D11ObjectManager::GetModel(std::string path)
	{
		auto find = mModels.find(path);
		assert(find != mModels.end());
		return find->second;
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
			}
		}

		mModels.erase(find);
	}

	bool D3D11ObjectManager::CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		auto find = mStaticMeshResources.find(key);

		if (find != mStaticMeshResources.end())
		{
			return false;
		}

		std::shared_ptr<StaticMesh> staticMesh = std::make_shared <StaticMesh>(device, meshData);
		mStaticMeshResources.insert({ key, staticMesh });

		return true;
	}
	bool D3D11ObjectManager::CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData)
	{
		auto find = mSkinnedMeshResources.find(key);

		if (find != mSkinnedMeshResources.end())
		{
			return false;
		}

		std::shared_ptr<SkinnedMesh> skinnedMesh = std::make_shared <SkinnedMesh>(device, meshData);
		mSkinnedMeshResources.insert({ key, skinnedMesh });

		return true;
	}
	bool D3D11ObjectManager::CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath)
	{
		auto find = mMaterialResources.find(key);

		if (find != mMaterialResources.end())
		{
			return false;
		}

		std::shared_ptr<Material> staticMesh = std::make_shared <Material>(device, matrialData, basePath);
		mMaterialResources.insert({ key, staticMesh });

		return true;
	}
	bool D3D11ObjectManager::CreateBoneHierarchy(std::string key, const fq::common::Model modelData)
	{
		return true;
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

		SkinnedMeshObject* skinnedMeshObject = new SkinnedMeshObject(findedStaticMesh->second, materials, info.Transform);
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