#include "D3D11ObjectManager.h"
#include "D3D11ModelManager.h"
#include "D3D11Device.h"
#include "RenderObject.h"
#include "Mesh.h"
#include "Material.h"
#include "BoneHierarchy.h"

#include "../FQLoader/ModelLoader.h"
#include "../FQLoader/ModelConverter.h"

namespace fq::graphics
{
	D3D11ObjectManager::~D3D11ObjectManager()
	{
		for (IStaticMeshObject* iStaticMeshObject : mStaticMeshObjects)
		{
			deleteIStaticMeshObject(iStaticMeshObject);
		}
		for (ISkinnedMeshObject* iSkinnedMeshObject : mSkinnedMeshObjects)
		{
			deleteISkinnedMeshObject(iSkinnedMeshObject);
		}
		while (!mStaticMeshDeleteQueue.empty())
		{
			deleteIStaticMeshObject(mStaticMeshDeleteQueue.front());
			mStaticMeshDeleteQueue.pop();
		}
		while (!mSkinnedMeshDeleteQueue.empty())
		{
			deleteISkinnedMeshObject(mSkinnedMeshDeleteQueue.front());
			mSkinnedMeshDeleteQueue.pop();
		}
	}

	void D3D11ObjectManager::AddAnimation(const std::shared_ptr<D3D11ModelManager>& modelManager, ISkinnedMeshObject* iSkinnedMeshObject, AnimationInfo info)
	{
		std::shared_ptr<fq::common::AnimationClip> animationResource = modelManager->FindAnimationOrNull(modelManager->GenerateAnimationKey(info.ModelPath, info.AnimationName));

		if (animationResource != nullptr)
		{
			SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(iSkinnedMeshObject);
			skinnedMeshObject->AddAnimation(info.AnimationKey, animationResource);
		}
	}

	IStaticMeshObject* D3D11ObjectManager::CreateStaticMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info)
	{
		std::shared_ptr<StaticMesh> staticMesh = modelManager->FindStaticMeshOrNull(modelManager->GenerateStaticMeshKey(info.ModelPath, info.MeshName));

		if (staticMesh == nullptr)
		{
			return nullptr;
		}

		std::vector<std::shared_ptr<Material>> materials;
		materials.reserve(info.MaterialNames.size());

		for (const std::string& materialName : info.MaterialNames)
		{
			std::shared_ptr<Material> material = modelManager->FindMaterialOrNull(modelManager->GenerateMaterialKey(info.ModelPath, materialName));
			materials.push_back(material);
		}

		StaticMeshObject* staticMeshObject = new StaticMeshObject(staticMesh, materials, info.Transform);
		mStaticMeshObjects.insert(staticMeshObject);

		return staticMeshObject;
	}
	void D3D11ObjectManager::DeleteStaticMeshObject(IStaticMeshObject* staticMeshObjectInterface)
	{
		mStaticMeshDeleteQueue.push(staticMeshObjectInterface);
		mStaticMeshObjects.erase(staticMeshObjectInterface);
	}

	ISkinnedMeshObject* D3D11ObjectManager::CreateSkinnedMeshObject(const std::shared_ptr<D3D11ModelManager>& modelManager, MeshObjectInfo info)
	{
		std::shared_ptr<SkinnedMesh> skinnedMesh = modelManager->FindSkinnedMeshOrNull(modelManager->GenerateStaticMeshKey(info.ModelPath, info.MeshName));

		if (skinnedMesh == nullptr)
		{
			return nullptr;
		}

		std::shared_ptr<BoneHierarchy> boneHierarchy = modelManager->FindBoneHierarchyOrNull(modelManager->GenerateBoneHierarachyKey(info.ModelPath));

		if (boneHierarchy == nullptr)
		{
			return nullptr;
		}

		BoneHierarchyCache boneHierarchyCache(boneHierarchy);

		std::vector<std::shared_ptr<Material>> materials;
		materials.reserve(info.MaterialNames.size());

		for (const std::string& materialName : info.MaterialNames)
		{
			std::shared_ptr<Material> material = modelManager->FindMaterialOrNull(modelManager->GenerateMaterialKey(info.ModelPath, materialName));
			materials.push_back(material);
		}

		SkinnedMeshObject* skinnedMeshObject = new SkinnedMeshObject(skinnedMesh, materials, info.Transform, std::move(boneHierarchyCache));
		mSkinnedMeshObjects.insert(skinnedMeshObject);

		return skinnedMeshObject;
	}
	void D3D11ObjectManager::DeleteSkinnedMeshObject(ISkinnedMeshObject* skinnedMeshObjectInterface)
	{
		mSkinnedMeshDeleteQueue.push(skinnedMeshObjectInterface);
		mSkinnedMeshObjects.erase(skinnedMeshObjectInterface);
	}

	void D3D11ObjectManager::DeletePushedObject()
	{
		while (!mStaticMeshDeleteQueue.empty())
		{
			deleteIStaticMeshObject(mStaticMeshDeleteQueue.front());
			mStaticMeshDeleteQueue.pop();
		}
		while (!mSkinnedMeshDeleteQueue.empty())
		{
			deleteISkinnedMeshObject(mSkinnedMeshDeleteQueue.front());
			mSkinnedMeshDeleteQueue.pop();
		}
	}

	void D3D11ObjectManager::deleteIStaticMeshObject(IStaticMeshObject* iStaticMeshObject) const
	{
		StaticMeshObject* staticMeshObject = static_cast<StaticMeshObject*>(iStaticMeshObject);
		delete staticMeshObject;
	}
	void D3D11ObjectManager::deleteISkinnedMeshObject(ISkinnedMeshObject* iSkinnedMeshObject) const
	{
		SkinnedMeshObject* skinnedMeshObject = static_cast<SkinnedMeshObject*>(iSkinnedMeshObject);
		delete skinnedMeshObject;
	}
}