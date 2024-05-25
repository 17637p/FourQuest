#pragma once

#include "CommonHeader.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11Device;

	class D3D11ModelManager;

	class IStaticMeshObject;
	class ISkinnedMeshObject;
	class ITerrainMeshObject;

	class D3D11ObjectManager
	{
	public:
		D3D11ObjectManager() = default;
		~D3D11ObjectManager();

		void DeletePushedObject();

		IStaticMeshObject* CreateStaticMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void AddAnimation(const std::shared_ptr<D3D11ModelManager>& modelManager, IStaticMeshObject* staticMeshObjectInterface, AnimationInfo info);
		void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface);

		ISkinnedMeshObject* CreateSkinnedMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void AddAnimation(const std::shared_ptr<D3D11ModelManager>& modelManager, ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info);
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface);

		ITerrainMeshObject* CreateTerrainMeshObject(
			const std::shared_ptr<D3D11Device>& device,
			const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void DeleteTerrainMeshObject(ITerrainMeshObject* terrainMeshObjectInterface);
		void SetTerrainMeshObject(const std::shared_ptr<D3D11Device>& device,
			ITerrainMeshObject* iTerrainMeshObject,
			const TerrainMaterialInfo& material);

		inline const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const;
		inline const std::set<ISkinnedMeshObject*>& GetSkinnedMeshObjects() const;
		inline const std::set<ITerrainMeshObject*>& GetTerrainMeshObjects() const;

	private:
		void deleteIStaticMeshObject(IStaticMeshObject* iStaticMeshObject) const;
		void deleteISkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) const;
		void deleteITerrainMeshObject(ITerrainMeshObject* iTerrainMeshObject) const;

	private:
		std::set<IStaticMeshObject*> mStaticMeshObjects;
		std::set<ISkinnedMeshObject*> mSkinnedMeshObjects;
		std::set<ITerrainMeshObject*> mTerrainMeshObjects;

		std::queue<IStaticMeshObject*> mStaticMeshDeleteQueue;
		std::queue<ISkinnedMeshObject*> mSkinnedMeshDeleteQueue;
		std::queue<ITerrainMeshObject*> mTerrainMeshDeleteQueue;
	};

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetStaticMeshObjects() const
	{
		return mStaticMeshObjects;
	}

	inline const std::set<ISkinnedMeshObject*>& D3D11ObjectManager::GetSkinnedMeshObjects() const
	{
		return mSkinnedMeshObjects;
	}

	inline const std::set<ITerrainMeshObject*>& D3D11ObjectManager::GetTerrainMeshObjects() const
	{
		return mTerrainMeshObjects;
	}
}
