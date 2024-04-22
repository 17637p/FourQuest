#pragma once

#include <map>
#include <set>
#include <memory>
#include <filesystem>

#include <directxtk/SimpleMath.h>
#include <d3d11.h>

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class IStaticMeshObject;
	class ISkinnedMeshObject;
	class StaticMesh;
	class SkinnedMesh;
	class Material;

	class D3D11ObjectManager
	{
	public:
		bool CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath);

		void DeleteStaticMesh(std::string key);
		void DeleteSkinnedMesh(std::string key);
		void DeleteMaterial(std::string key);

		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info);
		void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface);

		ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info);
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface);

		inline const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const;
		inline const std::set<ISkinnedMeshObject*>& GetSkinnedMeshObjects() const;

	private:
		void findMaterial(const std::string& modelPath,
			const std::vector<std::string>& materialKeys,
			std::vector<std::shared_ptr<Material>>* outMaterials) const;

	private:
		std::map<std::string, std::shared_ptr<StaticMesh>> mStaticMeshResources;
		std::map<std::string, std::shared_ptr<SkinnedMesh>> mSkinnedMeshResources;
		std::map<std::string, std::shared_ptr<Material>> mMaterialResources;

		std::set<IStaticMeshObject*> mStaticMeshObjects;
		std::set<ISkinnedMeshObject*> mSkinnedMeshObjects;
	};

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetStaticMeshObjects() const
	{
		return mStaticMeshObjects;
	}

	inline const std::set<ISkinnedMeshObject*>& D3D11ObjectManager::GetSkinnedMeshObjects() const
	{
		return mSkinnedMeshObjects;
	}
}
