#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <cmath>

#include "Mesh.h"
#include "BoneHierarchy.h"

#include "../FQCommon/IFQRenderObject.h"

namespace fq::common
{
	struct TerrainMaterial;
}

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;
	class TerrainMaterial;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			std::vector<std::shared_ptr<Material>> materials,
			DirectX::SimpleMath::Matrix transform);
		~StaticMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;
	};

#pragma region inlineFunc

	void StaticMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}

	inline void StaticMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void StaticMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}
	inline void StaticMeshObject::SetAlpha(float alpha)
	{
		mAlpha = std::clamp<float>(alpha, 0.f, 1.f);
	}
	inline void StaticMeshObject::SetUseShadow(bool bUseShadow)
	{
		mbUseShadow = bUseShadow;
	}

	inline const std::shared_ptr<StaticMesh>& StaticMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline EObjectRenderType StaticMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline float StaticMeshObject::GetAlpha() const
	{
		return mAlpha;
	}
	inline bool StaticMeshObject::GetUseShadow() const
	{
		return mbUseShadow;
	}
	inline DirectX::BoundingBox StaticMeshObject::GetRenderBoundingBox() const
	{
		return mStaticMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere StaticMeshObject::GetRenderBoundingSphere() const
	{
		return mStaticMesh->GetMeshData().GetRenderBoundingSphere;
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

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetAnimationTime(float timePos) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;
		inline virtual bool SetAnimationKey(const std::string& animationKey) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual float GetAnimationTime() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual std::set<std::string> GetAnimationKeys() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);
		inline const std::shared_ptr<SkinnedMesh>& GetSkinnedMesh() const;
		inline const std::vector<std::shared_ptr<Material>>& GetMaterials() const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetFinalTransforms() const;

	private:
		std::shared_ptr<SkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<Material>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		float mTimePos;
		BoneHierarchyCache mBoneHierarchyCache;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;
	};

#pragma region inlineFunc
	inline void SkinnedMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void SkinnedMeshObject::UpdateAnimationTime(float timePos)
	{
		mTimePos = timePos;
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
	inline void SkinnedMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline void SkinnedMeshObject::SetAnimationTime(float timePos)
	{
		mTimePos = timePos;
		mBoneHierarchyCache.Update(timePos);
	}
	inline void SkinnedMeshObject::SetObjectRenderType(EObjectRenderType renderType)
	{
		mObjectRenderType = renderType;
	}
	inline const DirectX::SimpleMath::Matrix& SkinnedMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline float SkinnedMeshObject::GetAnimationTime() const
	{
		return mTimePos;
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
	inline DirectX::BoundingBox SkinnedMeshObject::GetRenderBoundingBox() const
	{
		return mSkinnedMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere SkinnedMeshObject::GetRenderBoundingSphere() const
	{
		return mSkinnedMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline  EObjectRenderType SkinnedMeshObject::GetObjectRenderType() const
	{
		return mObjectRenderType;
	}
	inline float SkinnedMeshObject::GetAlpha() const
	{
		return mAlpha;
	}
	inline bool SkinnedMeshObject::GetUseShadow() const
	{
		return mbUseShadow;
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
	inline void SkinnedMeshObject::SetAlpha(float alpha)
	{
		mAlpha = std::clamp<float>(alpha, 0.f, 1.f);
	}
	inline void SkinnedMeshObject::SetUseShadow(bool bUseShadow)
	{
		mbUseShadow = bUseShadow;
	}

#pragma endregion

	class TerrainMeshObject : public ITerrainMeshObject
	{
	public:
		TerrainMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			DirectX::SimpleMath::Matrix transform);
		~TerrainMeshObject() = default;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		virtual void SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device,
			const fq::common::TerrainMaterial& terrainMaterial);

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline const std::shared_ptr<TerrainMaterial>& GetTerrainMaterial() const;

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::shared_ptr<TerrainMaterial> mMaterial;
		DirectX::SimpleMath::Matrix mTransform;
	};

#pragma region inlineFunc
	inline void TerrainMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline const std::shared_ptr<StaticMesh>& TerrainMeshObject::GetStaticMesh() const
	{
		return mStaticMesh;
	}
	inline DirectX::BoundingBox TerrainMeshObject::GetRenderBoundingBox() const
	{
		return mStaticMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere TerrainMeshObject::GetRenderBoundingSphere() const
	{
		return mStaticMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline const std::shared_ptr<TerrainMaterial>& TerrainMeshObject::GetTerrainMaterial() const
	{
		return mMaterial;
	}
	inline const DirectX::SimpleMath::Matrix& TerrainMeshObject::GetTransform() const
	{
		return mTransform;
	}
#pragma endregion inlineFunc
}

