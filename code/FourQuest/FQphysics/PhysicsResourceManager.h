#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "ResourceBase.h"

namespace fq::physics
{
	class PhysicsResourceManager
	{
	public:
		PhysicsResourceManager();
		~PhysicsResourceManager();

		template <typename T>
		std::shared_ptr<T> Find(const std::string& path);

		template <typename T, typename ...Args>
		std::shared_ptr<T> Create(const std::string& path, Args... args);

	private:
		std::unordered_map<std::string, ResourceBase> mResourceContainer;
	};

	template<typename T>
	inline std::shared_ptr<T> PhysicsResourceManager::Find(const std::string& path)
	{


		return std::shared_ptr<T>();
	}

	template<typename T, typename ...Args>
	inline std::shared_ptr<T> PhysicsResourceManager::Create(const std::string& path, Args... args)
	{
		return std::shared_ptr<T>();
	}
}