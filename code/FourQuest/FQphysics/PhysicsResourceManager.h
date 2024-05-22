#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <array>

#include "ResourceBase.h"
#include <cassert>

namespace fq::physics
{
	class MaterialResource;
	class ConvexMeshResource;

	class PhysicsResourceManager
	{
	public:
		PhysicsResourceManager();
		~PhysicsResourceManager();

		template <typename T>
		void* Find(const std::string& name);

		template <typename T, typename ...Params>
		void* Create(const std::string& name, Params... params);

	private:
		template <typename T>
		const EResourceType& GetResourceType();

	private:
		using KeyObjMap = std::unordered_map<std::string, std::shared_ptr<ResourceBase>>;
		std::array<KeyObjMap, RESOURCE_TYPE_COUNT> mResourceContainer;
	};

	template<typename T>
	inline void* PhysicsResourceManager::Find(const std::string& path)
	{
		bool bIsBase = std::is_base_of<ResourceBase, T>::value;
		assert(bIsBase == true);

		EResourceType resourceType = GetResourceType<MaterialResource>();
		auto resource = mResourceContainer[static_cast<int>(resourceType)].find(path);

		if (resource != mResourceContainer[static_cast<int>(resourceType)].end())
			return resource->second.get();
		else
			return nullptr;
	}
	template<typename T, typename ...Params>
	inline void* PhysicsResourceManager::Create(const std::string& path, Params... params)
	{
		bool bIsBase = std::is_base_of<ResourceBase, T>::value;
		assert(bIsBase == true);

		EResourceType resourceType = GetResourceType<MaterialResource>();
		auto resoureceIter = mResourceContainer[static_cast<int>(resourceType)].find(path);

		if (resoureceIter != mResourceContainer[static_cast<int>(resourceType)].end())
		{
			return resoureceIter->second.get();
		}
		else
		{
			std::shared_ptr<T> resource = std::make_shared<T>(params...);
			mResourceContainer[static_cast<int>(resourceType)].insert(path, resource);
			return resource.get();
		}
	}
	template<typename T>
	inline const EResourceType& PhysicsResourceManager::GetResourceType()
	{
		if (std::is_same_v<T, MaterialResource>)
			return EResourceType::MATERIAL;
		if (std::is_same_v<T, ConvexMeshResource>)
			return EResourceType::CONVEX_MESH;

		assert(false);
		return EResourceType::END;
	}
}