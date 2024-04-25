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
	class BoneHierarchy;

	class D3D11ObjectManager
	{
	public:
		void ConvertModel(std::string fbxFile, std::string fileName);
		const fq::common::Model& CreateModel(const std::shared_ptr<D3D11Device>& device, std::string fileName, std::filesystem::path textureBasePath = "");
		void DeleteModel(std::string path);
		const fq::common::Model& GetModel(std::string path);

		// ModelResource는 별도의 매니저를 구성해야할 거 같다 느껴지네
		template <typename Resource, typename... Args>
		bool CreateResource(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key, Args... args);
		bool CreateStaticMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateSkinnedMesh(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Mesh& meshData);
		bool CreateMaterial(const std::shared_ptr<D3D11Device>& device, std::string key, const fq::common::Material& matrialData, std::filesystem::path basePath);
		bool CreateBoneHierarchy(std::string key, const fq::common::Model modelData);
		bool CreateAnimation(std::string key, const fq::common::AnimationClip& animationData);

		void DeleteStaticMesh(std::string key);
		void DeleteSkinnedMesh(std::string key);
		void DeleteMaterial(std::string key);
		void DeleteBoneHierarchy(std::string key);
		void DeleteAnimation(std::string key);

		std::shared_ptr<fq::common::AnimationClip> FindAnimationResource(const std::string& key);
		// to do : FindResource()
		 
		// to do : GenerateResourceKey() 

		// to do : Job은 랜더 오브젝트의 래퍼런스를 들고 있는 형태라 랜더 오브젝트의 삭제는 지연처리되어야 한다.
		IStaticMeshObject* CreateStaticMeshObject(MeshObjectInfo info);
		void DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface);

		ISkinnedMeshObject* CreateSkinnedMeshObject(MeshObjectInfo info);
		void AddAnimation(ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info);
		void DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface);

		inline const std::set<IStaticMeshObject*>& GetStaticMeshObjects() const;
		inline const std::set<ISkinnedMeshObject*>& GetSkinnedMeshObjects() const;

	private:
		void findMaterial(const std::string& modelPath,
			const std::vector<std::string>& materialKeys,
			std::vector<std::shared_ptr<Material>>* outMaterials) const;

	private:
		std::map<std::string, fq::common::Model> mModels;

		std::map<std::string, std::shared_ptr<StaticMesh>> mStaticMeshResources;
		std::map<std::string, std::shared_ptr<SkinnedMesh>> mSkinnedMeshResources;
		std::map<std::string, std::shared_ptr<Material>> mMaterialResources;
		std::map<std::string, std::shared_ptr<BoneHierarchy>> mBoneHierarchyResources;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationClipResources;

		std::set<IStaticMeshObject*> mStaticMeshObjects;
		std::set<ISkinnedMeshObject*> mSkinnedMeshObjects;
	};

	template <typename Resource, typename... Args>
	bool D3D11ObjectManager::CreateResource(std::map<std::string, std::shared_ptr<Resource>>& storage, std::string key, Args... args)
	{
		auto find = storage.find(key);

		if (find != storage.end())
		{
			return false;
		}

		std::shared_ptr<Resource> resource = std::make_shared<Resource>(args...);
		storage.insert({ key, resource });

		return true;
	}

	inline const std::set<IStaticMeshObject*>& D3D11ObjectManager::GetStaticMeshObjects() const
	{
		return mStaticMeshObjects;
	}

	inline const std::set<ISkinnedMeshObject*>& D3D11ObjectManager::GetSkinnedMeshObjects() const
	{
		return mSkinnedMeshObjects;
	}
}
