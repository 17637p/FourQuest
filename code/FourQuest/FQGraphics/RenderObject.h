#pragma once

#include <cassert>
#include <memory>
#include <vector>
#include "BoneHierarchy.h"
#include "../FQCommon/IFQRenderObject.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform);
		~StaticMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		EObjectRenderType mObjectRenderType;
	};

#pragma region inlineFunc

	void StaticMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}

	inline void StaticMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}

	inline const std::shared_ptr<StaticMesh>& StaticMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline EObjectRenderType StaticMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline const std::vector<std::shared_ptr<Material>>& StaticMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const DirectX::SimpleMath::Matrix& StaticMeshObject::GetTransform() const
	{
		return mTransform;
	}
#pragma endregion

	class SkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		SkinnedMeshObject(std::shared_ptr<SkinnedMesh> SkinnedMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform,
			BoneHierarchyCache boneHierarchyCache);
		~SkinnedMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void UpdateAnimationTime(float timePos) override;

		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);

		inline virtual bool SetAnimationKey(const std::string& animationKey) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual std::set<std::string> GetAnimationKeys() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline const std::shared_ptr<SkinnedMesh>& GetSkinnedMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetFinalTransforms() const;

	private:
		std::shared_ptr<SkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		BoneHierarchyCache mBoneHierarchyCache;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		EObjectRenderType mObjectRenderType;
	};

#pragma region inlineFunc
	inline void SkinnedMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void SkinnedMeshObject::UpdateAnimationTime(float timePos)
	{
		mBoneHierarchyCache.Update(timePos);
	}

	inline void SkinnedMeshObject::AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip)
	{
		mAnimationMap.insert({ animationKey, animationClip });
	}

	inline bool SkinnedMeshObject::SetAnimationKey(const std::string& animationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		mBoneHierarchyCache.SetAnimation(find->second);

		return true;
	}
	inline void SkinnedMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}
	inline const DirectX::SimpleMath::Matrix& SkinnedMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline  std::set<std::string> SkinnedMeshObject::GetAnimationKeys() const
	{
		std::set<std::string> animationKeys;

		for (const auto& [name, clip] : mAnimationMap)
		{
			animationKeys.insert(name);
		}

		return animationKeys;
	}
	inline  EObjectRenderType SkinnedMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline const std::shared_ptr<SkinnedMesh>& SkinnedMeshObject::GetSkinnedMesh() const
	{
		return mSkinnedMesh;
	}
	inline const std::vector<std::shared_ptr<Material>>& SkinnedMeshObject::GetMaterials() const
	{
		return mMaterials;
	}
	inline const std::vector<DirectX::SimpleMath::Matrix>& SkinnedMeshObject::GetFinalTransforms() const
	{
		return mBoneHierarchyCache.GetFinalTransforms();
	}
#pragma endregion
}

