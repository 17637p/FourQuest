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


	class D3D11ObjectManager
	{
	public:
		bool CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath);

		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info);
		void DeleteMeshObject(IStaticMeshObject* meshObject);

		inline const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const;

	private:
		std::map<std::string, std::shared_ptr<class StaticMesh>> mStaticMeshResources;
		std::map<std::string, std::shared_ptr<class Material>> mMaterialResources;

		std::set<IStaticMeshObject*> mStaticMeshObjects;
	};

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetStaticMeshObjects() const
	{
		return mStaticMeshObjects;
	}
}
