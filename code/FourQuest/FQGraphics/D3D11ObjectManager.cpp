#include "D3D11Device.h"
#include "D3D11ObjectManager.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"

namespace fq::graphics
{
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

	IStaticMeshObject* D3D11ObjectManager::CreateStaticMeshObject(MeshObjectInfo info)
	{
		auto findedStaticMesh = mStaticMeshResources.find(info.MeshKey);

		if (findedStaticMesh == mStaticMeshResources.end())
		{
			return nullptr;
		}

		std::vector<std::shared_ptr<Material>> materials;

		for (const std::string& materialKey : info.MaterialKeys)
		{
			auto findedMaterial = mMaterialResources.find(materialKey);

			if (findedMaterial != mMaterialResources.end())
			{
				materials.push_back(findedMaterial->second);
			}
			else
			{
				materials.push_back(nullptr);
			}
		}

		StaticMeshObject* staticMeshObject = new StaticMeshObject(findedStaticMesh->second, materials, info.Transform);
		mStaticMeshObjects.insert(staticMeshObject);

		return staticMeshObject;
	}
	void D3D11ObjectManager::DeleteMeshObject(IStaticMeshObject* meshObject)
	{
		mStaticMeshObjects.erase(meshObject);

		StaticMeshObject* staticMeshObject = static_cast<StaticMeshObject*>(meshObject);
		delete staticMeshObject;
	}
}