#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <array>

#include "ResourceBase.h"
#include "PxPhysicsAPI.h"
#include <cassert>

namespace fq::physics
{
	class MaterialResource;
	class ConvexMeshResource;
	class TriangleMeshResource;
	class HeightFieldResource;

	class PhysicsResourceManager
	{
	public:
		PhysicsResourceManager();
		~PhysicsResourceManager();

		bool Initialize(physx::PxPhysics* physics);

		template <typename T>
		std::shared_ptr<T> Find(const unsigned int& hash);
		template <typename T, typename ...Params>
		std::shared_ptr<T> Create(const unsigned int& hash, Params... params);

	private:
		template <typename T>
		const EResourceType& getResourceType();

	private:
		physx::PxPhysics* mPhysics;

		using KeyObjMap = std::unordered_map<unsigned int, std::shared_ptr<ResourceBase>>;
		std::array<KeyObjMap, RESOURCE_TYPE_COUNT> mResourceContainer;
	};


	template<typename T>
	inline std::shared_ptr<T> PhysicsResourceManager::Find(const unsigned int& hash)
	{
		bool bIsBase = std::is_base_of<ResourceBase, T>::value;
		assert(bIsBase == true);

		EResourceType resourceType = getResourceType<MaterialResource>();
		auto resource = mResourceContainer[static_cast<int>(resourceType)].find(hash);

		if (resource != mResourceContainer[static_cast<int>(resourceType)].end())
			return std::dynamic_pointer_cast<T>(resource->second);
		else
			return nullptr;
	}

	template<typename T, typename ...Params>
	inline std::shared_ptr<T> PhysicsResourceManager::Create(const unsigned int& hash, Params... params)
	{
		bool bIsBase = std::is_base_of<ResourceBase, T>::value;
		assert(bIsBase == true);

		EResourceType resourceType = getResourceType<T>();
		auto resoureceIter = mResourceContainer[static_cast<int>(resourceType)].find(hash);

		if (resoureceIter != mResourceContainer[static_cast<int>(resourceType)].end())
		{
			return std::dynamic_pointer_cast<T>(resoureceIter->second);
		}
		else
		{
			std::shared_ptr<T> resource = std::make_shared<T>(mPhysics, params...);
			mResourceContainer[static_cast<int>(resourceType)].insert(std::make_pair(hash, resource));
			return resource;
		}
	}

	template<typename T>
	inline const EResourceType& PhysicsResourceManager::getResourceType()
	{
		if (std::is_same_v<T, ConvexMeshResource>)
			return EResourceType::CONVEX_MESH;
		if (std::is_same_v<T, TriangleMeshResource>)
			return EResourceType::TRIANGLE_MESH;
		if (std::is_same_v<T, HeightFieldResource>)
			return EResourceType::HEIGHT_FIELD;

		return EResourceType::END;
	}
}