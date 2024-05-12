#pragma once

#include "CommonHeader.h"

#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class D3D11ModelManager;
	class IStaticMeshObject;
	class ISkinnedMeshObject;

	class D3D11ObjectManager
	{
	public:
		D3D11ObjectManager() = default;
		~D3D11ObjectManager();

		void DeletePushedObject();

		IStaticMeshObject* CreateStaticMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface);

		ISkinnedMeshObject* CreateSkinnedMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void AddAnimation(const std::shared_ptr<D3D11ModelManager>& modelManager, ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info);
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface);

		IStaticMeshObject* CreateTerrainMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info);
		void DeleteTerrainMeshObject(IStaticMeshObject* terrainMeshObjectInterface);

		inline const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const;
		inline const std::set<ISkinnedMeshObject*>& GetSkinnedMeshObjects() const;
		inline const std::set<IStaticMeshObject*>& GetTerrainMeshObjects() const;

	private:
		void deleteIStaticMeshObject(IStaticMeshObject* iStaticMeshObject) const;
		void deleteISkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) const;
		void deleteTerrainMeshObject(IStaticMeshObject* iStaticMeshObject) const;

	private:
		std::set<IStaticMeshObject*> mStaticMeshObjects;
		std::set<ISkinnedMeshObject*> mSkinnedMeshObjects;
		std::set<IStaticMeshObject*> mTerrainMeshObjects;

		std::queue<IStaticMeshObject*> mStaticMeshDeleteQueue;
		std::queue<ISkinnedMeshObject*> mSkinnedMeshDeleteQueue;
		std::queue<IStaticMeshObject*> mTerrainMeshDeleteQueue;
	};

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetStaticMeshObjects() const
	{
		return mStaticMeshObjects;
	}

	inline const std::set<ISkinnedMeshObject*>& D3D11ObjectManager::GetSkinnedMeshObjects() const
	{
		return mSkinnedMeshObjects;
	}

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetTerrainMeshObjects() const
	{
		return mTerrainMeshObjects;
	}
}
