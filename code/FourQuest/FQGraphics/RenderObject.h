#pragma once

#include <algorithm>
#include <memory>
#include <array>
#include <vector>
#include <cmath>
#include <d3d11.h>
#include <wrl.h>

#include "Mesh.h"
#include "BoneHierarchy.h"
#include "AnimationHelper.h"
#include "D3D11Texture.h"
#include "D3D11Device.h"

#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/FQCommonGraphics.h"

namespace fq::graphics
{
	class StaticMesh;
	class SkinnedMesh;
	class Material;
	class TerrainMaterial;
	struct TerrainMaterialInfo;

	class StaticMeshObject : public IStaticMeshObject
	{
	public:
		StaticMeshObject(std::shared_ptr<StaticMesh> staticMesh,
			std::vector<std::shared_ptr<IMaterial>> materials,
			DirectX::SimpleMath::Matrix transform);
		~StaticMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;
		inline virtual void SetAnimationTime(float timePos) override;
		inline virtual bool SetAnimationKey(const std::string& animationKey) override;
		inline virtual bool SetBlendAnimationKey(const std::array<std::string, 2> animationKeys) override;
		inline virtual bool SetBlendAnimationKey(const std::string& lhsAnimationKey, const std::string& rhsAnimationKey) override;
		inline virtual void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) override;
		inline virtual void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;
		inline virtual float GetAnimationTime() const override;
		inline virtual float GetBlendTime() const override;

		inline const std::shared_ptr<StaticMesh>& GetStaticMesh() const;
		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);

		// Outline
		virtual void SetOutlineColor(const DirectX::SimpleMath::Color& color) override;
		virtual DirectX::SimpleMath::Color GetOutlineColor() const override;

		virtual const std::vector<std::shared_ptr<IMaterial>>& GetMaterialInterfaces() const { return mMaterials; }
		virtual void SetMaterialInterfaces(const std::vector<std::shared_ptr<IMaterial>>& materialInterfaces) { mMaterials = materialInterfaces; }

		// Mesh
		virtual const fq::common::Mesh& GetMeshData() const override { return mStaticMesh->GetMeshData(); }

		// Decal
		virtual void SetIsAppliedDecal(bool bIsAppiedDecal) override { mbIsAppliedDecal = bIsAppiedDecal; }
		virtual const bool GetIsAppliedDecal() const override { return mbIsAppliedDecal; }

	private:
		std::shared_ptr<StaticMesh> mStaticMesh;
		std::vector<std::shared_ptr<IMaterial>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;

		float mTimePos;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		const fq::common::NodeClip* mNodeCache;
		std::shared_ptr<fq::common::AnimationClip> mClipCache;

		const fq::common::NodeClip* mBlendNodeCache;
		std::shared_ptr<fq::common::AnimationClip> mBlendClipCache;
		float mBlendTimePos;

		DirectX::SimpleMath::Color mOutLineColor;

		bool mbIsAppliedDecal;
	};

#pragma region inlineFunc

	void StaticMeshObject::UpdateTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		SetTransform(transform);
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
	inline void StaticMeshObject::SetAnimationTime(float timePos)
	{
		mTimePos = timePos;

		if (mNodeCache != nullptr && mClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mNodeCache->Keyframes, *mClipCache, timePos, &lhs, &rhs, &weight);
			fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			mTransform = AnimationHelper::CreateMatrix(keyframe);
		}
	}
	inline bool StaticMeshObject::SetAnimationKey(const std::string& animationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		const std::vector<fq::common::NodeClip>& nodeClips = find->second->NodeClips;
		auto nodeClip = std::find_if(nodeClips.begin(), nodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });

		mClipCache = find->second;

		if (nodeClip != nodeClips.end())
		{
			mNodeCache = &(*nodeClip);
		}

		return true;
	}
	inline bool StaticMeshObject::SetBlendAnimationKey(const std::array<std::string, 2> animationKeys)
	{
		return SetBlendAnimationKey(animationKeys[0], animationKeys[1]);
	}
	inline bool StaticMeshObject::SetBlendAnimationKey(const std::string& animationKey, const std::string& blendAnimationKey)
	{
		auto find = mAnimationMap.find(animationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		auto secondFind = mAnimationMap.find(blendAnimationKey);

		if (secondFind == mAnimationMap.end())
		{
			return false;
		}

		const std::vector<fq::common::NodeClip>& nodeClips = find->second->NodeClips;
		auto nodeClip = std::find_if(nodeClips.begin(), nodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });
		mClipCache = find->second;

		if (nodeClip != nodeClips.end())
		{
			mNodeCache = &(*nodeClip);
		}

		const std::vector<fq::common::NodeClip>& secondNodeClips = secondFind->second->NodeClips;
		auto secondNodeClip = std::find_if(secondNodeClips.begin(), secondNodeClips.end(), [&](const fq::common::NodeClip& nodeClip) { return nodeClip.NodeName == mStaticMesh->GetMeshData().NodeName; });
		mBlendClipCache = secondFind->second;

		if (secondNodeClip != secondNodeClips.end())
		{
			mBlendNodeCache = &(*secondNodeClip);
		}

		return true;
	}
	inline void StaticMeshObject::SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight)
	{
		SetBlendAnimationTime(timePos[0], timePos[1], blendWeight);
	}
	inline void StaticMeshObject::SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight)
	{
		mTimePos = timePos;
		mBlendTimePos = blendTimePos;

		if (mNodeCache == nullptr && mBlendNodeCache == nullptr)
		{
			return;
		}

		blendWeight = std::min<float>(blendWeight, 1.f);
		blendWeight = std::max<float>(blendWeight, 0.f);

		fq::common::Keyframe srcAnimation;
		fq::common::Keyframe destAnimation;

		if (mNodeCache != nullptr && mClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mNodeCache->Keyframes, *mClipCache, timePos, &lhs, &rhs, &weight);
			srcAnimation = AnimationHelper::Interpolate(lhs, rhs, weight);
		}

		if (mBlendNodeCache != nullptr && mBlendClipCache != nullptr)
		{
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			AnimationHelper::FindKeyframe(mBlendNodeCache->Keyframes, *mBlendClipCache, timePos, &lhs, &rhs, &weight);
			destAnimation = AnimationHelper::Interpolate(lhs, rhs, weight);
		}

		fq::common::Keyframe resultKeyframe = AnimationHelper::Interpolate(srcAnimation, destAnimation, blendWeight);
		mTransform = AnimationHelper::CreateMatrix(resultKeyframe);
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
	inline float StaticMeshObject::GetAnimationTime() const
	{
		return mTimePos;
	}
	inline float StaticMeshObject::GetBlendTime() const
	{
		return mBlendTimePos;
	}
	inline const DirectX::SimpleMath::Matrix& StaticMeshObject::GetTransform() const
	{
		return mTransform;
	}
	inline void StaticMeshObject::AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip)
	{
		mAnimationMap.insert({ animationKey, animationClip });
	}

#pragma endregion

	class SkinnedMeshObject : public ISkinnedMeshObject
	{
	public:
		SkinnedMeshObject(std::shared_ptr<SkinnedMesh> SkinnedMesh,
			std::vector<std::shared_ptr<IMaterial>> materials,
			DirectX::SimpleMath::Matrix transform,
			BoneHierarchyCache boneHierarchyCache);
		~SkinnedMeshObject() = default;

		inline virtual void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void UpdateAnimationTime(float timePos) override;

		inline virtual void SetBindPose() override;
		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		inline virtual void SetAnimationTime(float timePos) override;
		inline virtual void SetObjectRenderType(EObjectRenderType renderType) override;
		inline virtual void SetAlpha(float alpha) override;
		inline virtual void SetUseShadow(bool bUseShadow) override;
		inline virtual bool SetAnimationKey(const std::string& animationKey) override;
		inline virtual bool SetBlendAnimationKey(const std::array<std::string, 2> animKey) override;
		inline virtual bool SetBlendAnimationKey(const std::string& animKey, const std::string& blendAnimKey) override;
		inline virtual void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) override;
		inline virtual void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) override;

		inline virtual const DirectX::SimpleMath::Matrix& GetTransform() const override;
		inline virtual float GetAnimationTime() const override;
		inline virtual EObjectRenderType GetObjectRenderType() const override;
		inline virtual float GetAlpha() const override;
		inline virtual bool GetUseShadow() const override;
		inline virtual std::set<std::string> GetAnimationKeys() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;
		inline virtual float GetBlendTime() const override;

		inline void AddAnimation(std::string animationKey, std::shared_ptr<fq::common::AnimationClip> animationClip);
		inline const std::shared_ptr<SkinnedMesh>& GetSkinnedMesh() const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetTransposedFinalTransforms() const;

		// Outline
		virtual void SetOutlineColor(const DirectX::SimpleMath::Color& color) override;
		virtual DirectX::SimpleMath::Color GetOutlineColor() const override;

		virtual const std::vector<std::shared_ptr<IMaterial>>& GetMaterialInterfaces() const { return mMaterials; }
		virtual void SetMaterialInterfaces(const std::vector<std::shared_ptr<IMaterial>>& materials) { mMaterials = materials; }

		// Bone
		virtual const std::vector<fq::common::Bone>& GetBones() const override;
		virtual unsigned int GetBoneIndex(const std::string& boneName) const;
		virtual bool TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex);
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const;
		virtual bool TryGetRootTransform(const std::string& boneName, DirectX::SimpleMath::Matrix* outRootTransform);
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const;
		virtual const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const;

		// Mesh
		virtual const fq::common::Mesh& GetMeshData() const override { return mSkinnedMesh->GetMeshData(); }

		// Decal
		virtual void SetIsAppliedDecal(bool bIsAppiedDecal) override { mbIsAppliedDecal = bIsAppiedDecal; }
		virtual const bool GetIsAppliedDecal() const override { return mbIsAppliedDecal; }


	private:
		std::shared_ptr<SkinnedMesh> mSkinnedMesh;
		std::vector<std::shared_ptr<IMaterial>> mMaterials;
		DirectX::SimpleMath::Matrix mTransform;
		float mTimePos;
		float mBlendTimePos;
		BoneHierarchyCache mBoneHierarchyCache;
		std::map<std::string, std::shared_ptr<fq::common::AnimationClip>> mAnimationMap;
		EObjectRenderType mObjectRenderType;
		float mAlpha;
		bool mbUseShadow;
		DirectX::SimpleMath::Color mOutLineColor;
		bool mbIsAppliedDecal;
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
	inline bool SkinnedMeshObject::SetBlendAnimationKey(const std::array<std::string, 2> animationKeys)
	{
		return SetBlendAnimationKey(animationKeys[0], animationKeys[1]);
	}
	inline bool SkinnedMeshObject::SetBlendAnimationKey(const std::string& srcAnimationKey, const std::string& destAnimationKey)
	{
		auto find = mAnimationMap.find(srcAnimationKey);

		if (find == mAnimationMap.end())
		{
			return false;
		}

		auto secondFind = mAnimationMap.find(destAnimationKey);

		if (secondFind == mAnimationMap.end())
		{
			return false;
		}

		mBoneHierarchyCache.SetAnimation(find->second, secondFind->second);

		return true;
	}

	inline void SkinnedMeshObject::SetBindPose()
	{
		mTimePos = 0.f;
		mBoneHierarchyCache.SetBindPose();
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
	inline void SkinnedMeshObject::SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight)
	{
		SetBlendAnimationTime(timePos[0], timePos[1], blendWeight);
	}
	inline void SkinnedMeshObject::SetBlendAnimationTime(float lhsTimePos, float rhsTimePos, float blendWeight)
	{
		mTimePos = lhsTimePos;
		mBlendTimePos = rhsTimePos;
		blendWeight = std::min<float>(blendWeight, 1.f);
		blendWeight = std::max<float>(blendWeight, 0.f);
		mBoneHierarchyCache.Update(mTimePos, mBlendTimePos, blendWeight);
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
	inline float SkinnedMeshObject::GetBlendTime() const
	{
		return mBlendTimePos;
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
	inline const std::vector<DirectX::SimpleMath::Matrix>& SkinnedMeshObject::GetTransposedFinalTransforms() const
	{
		return mBoneHierarchyCache.GetTransposedFinalTransforms();
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
		TerrainMeshObject(
			const std::shared_ptr<D3D11Device>& device,
			std::shared_ptr<StaticMesh> staticMesh,
			DirectX::SimpleMath::Matrix transform);
		~TerrainMeshObject() = default;

		inline virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override;
		void SetTerrainMaterial(const std::shared_ptr<D3D11Device>& device,
			const TerrainMaterialInfo& terrainMaterial);

		inline virtual DirectX::SimpleMath::Matrix GetTransform() const override;
		inline virtual DirectX::BoundingBox GetRenderBoundingBox() const override;
		inline virtual DirectX::BoundingSphere GetRenderBoundingSphere() const override;

		inline const std::shared_ptr<TerrainMesh>& GetTerrainMesh() const;
		inline const std::shared_ptr<TerrainMaterial>& GetTerrainMaterial() const;

		size_t GetNumIndices() const { return mNumIndices; }

	private:
		// Terrain Vertex, Index 수정
		void BuildTerrainMesh(const std::shared_ptr<D3D11Device>& device, std::shared_ptr<StaticMesh> staticMesh);

		float GetWidth() const;
		float GetDepth() const;
		void BuildQuadPatchVB(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, fq::common::Mesh& mesh);
		void BuildQuadPatchIB(fq::common::Mesh& mesh);

		void CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY);
		void CalcAllPatchBoundsY(std::vector<DirectX::SimpleMath::Vector2>& patchBoundsY, UINT i, UINT j);
		void CalcNormalTangent(fq::common::Mesh& mesh);

	private:
		std::shared_ptr<StaticMesh> mTempStaticMesh; // Plane
		std::shared_ptr<TerrainMesh> mTerrainMesh;
		std::shared_ptr<TerrainMaterial> mMaterial;

		size_t mNumIndices;
		unsigned int mWidth;
		unsigned int mHeight;

		UINT mNumPatchVertRows;
		UINT mNumPatchVertCols;

		UINT mNumPatchVertices;
		UINT mNumPatchQuadFaces;

		unsigned int mCellsPerPatch;

		DirectX::SimpleMath::Matrix mTransform;
	};

#pragma region inlineFunc
	inline void TerrainMeshObject::SetTransform(const DirectX::SimpleMath::Matrix& transform)
	{
		mTransform = transform;
	}
	inline const std::shared_ptr<TerrainMesh>& TerrainMeshObject::GetTerrainMesh() const
	{
		return mTerrainMesh;
	}
	inline DirectX::BoundingBox TerrainMeshObject::GetRenderBoundingBox() const
	{
		return mTerrainMesh->GetMeshData().RenderBoundingBox;
	}
	inline DirectX::BoundingSphere TerrainMeshObject::GetRenderBoundingSphere() const
	{
		return mTerrainMesh->GetMeshData().GetRenderBoundingSphere;
	}
	inline const std::shared_ptr<TerrainMaterial>& TerrainMeshObject::GetTerrainMaterial() const
	{
		return mMaterial;
	}
	inline DirectX::SimpleMath::Matrix TerrainMeshObject::GetTransform() const
	{
		// 이동 성분만 뽑아서 반환
		DirectX::SimpleMath::Vector3 translateVec(mTransform._41, mTransform._42, mTransform._43);
		DirectX::SimpleMath::Matrix matrix = DirectX::SimpleMath::Matrix::CreateTranslation(translateVec);

		return matrix;
	}
#pragma endregion inlineFunc

	class ImageObject : public IImageObject
	{
	public:
		ImageObject();
		virtual ~ImageObject();

		virtual void SetStartX(float startX) override;
		virtual void SetStartY(float startY) override;
		virtual float GetStartX() override;
		virtual float GetStartY() override;

		virtual void SetWidth(float width) override;
		virtual void SetHeight(float height) override;
		virtual float GetWidth() override;
		virtual float GetHeight() override;

		virtual void SetLayer(unsigned int layer) override;
		virtual unsigned int GetLayer() override;

		virtual void SetXRatio(float xRatio) override;
		virtual float GetXRatio() override;
		virtual void SetYRatio(float yRatio) override;
		virtual float GetYRatio() override;

		virtual void SetAlpha(float alpha) override;
		virtual float GetAlpha() override;

		virtual void SetRotation(float angle) override;
		virtual float GetRotation() const override;

		virtual void SetScaleX(float scaleX) override;
		virtual void SetScaleY(float scaleY) override;
		virtual float GetScaleX() const override;
		virtual float GetScaleY() const override;

		virtual void SetImagePath(const std::string& texturePath) override;
		virtual std::string GetImagePath() override;

	protected:
		float mStartX;
		float mStartY;
		float mWidth;
		float mHeight;

		float mXRatio;
		float mYRatio;

		float mAlpha;

		unsigned int mLayer;

		float mRotationAngle;
		float mScaleX;
		float mScaleY;

		std::string mImagePath;
	};
}

