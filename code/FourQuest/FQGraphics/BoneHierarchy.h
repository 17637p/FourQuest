#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <directxtk/SimpleMath.h>
#include "../FQCommon/FQCommonLoader.h"
#include "AnimationHelper.h"

namespace fq::graphics
{
	class BoneHierarchy
	{
	public:
		BoneHierarchy(fq::common::Model model);
		~BoneHierarchy() = default;

		inline const std::vector<fq::common::Bone>& GetBones() const;
		inline size_t GetBoneCount() const;
		unsigned int GetBoneIndex(const std::string& boneName) const;
		bool TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex);

	private:
		std::vector<fq::common::Bone> mBones;
	};

	inline const std::vector<fq::common::Bone>& BoneHierarchy::GetBones() const
	{
		return mBones;
	}
	inline size_t BoneHierarchy::GetBoneCount() const
	{
		return mBones.size();
	}

	class BoneHierarchyCache
	{
	public:
		using BoneNodeClipCache = std::pair<const struct fq::common::Bone*, const struct fq::common::NodeClip*>;

	public:
		BoneHierarchyCache(std::shared_ptr<BoneHierarchy> boneHierarchy);
		~BoneHierarchyCache() = default;

		void Clear();
		void Update(float timePos);
		void Update(float timePos, float blendTimePos, float weight);

		inline void SetAnimation(std::shared_ptr<fq::common::AnimationClip> animationCilp);
		inline void SetAnimation(std::shared_ptr<fq::common::AnimationClip> animationCilp, std::shared_ptr<fq::common::AnimationClip> blendAnimationClip);

		inline const std::shared_ptr<BoneHierarchy>& GetBoneHierarchy() const;
		inline const std::shared_ptr<fq::common::AnimationClip>& GetAnimationClip() const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetTransposedFinalTransforms() const;
		// 캐싱 데이터의 getter는 필요할까?

		inline const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const;
		inline bool TryGetRootTransform(const std::string& boneName, DirectX::SimpleMath::Matrix* outRootTransform);
		inline const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const;
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const;

	private:
		void link();
		void linkBlendAnim();

		inline bool checkValid() const;

	private:
		std::shared_ptr<BoneHierarchy> mBoneHierarchy;
		std::shared_ptr<fq::common::AnimationClip> mAnimationClip;
		std::shared_ptr<fq::common::AnimationClip> mBlendAnimationClip;
		std::vector<BoneNodeClipCache> mBoneNodeClipCache;
		std::vector<BoneNodeClipCache> mBlendBoneNodeClipCache;
		std::vector<DirectX::SimpleMath::Matrix> mRootTransforms;
		std::vector<DirectX::SimpleMath::Matrix> mTransposedFinalTransforms;
	};

	inline void BoneHierarchyCache::SetAnimation(std::shared_ptr<fq::common::AnimationClip> animationCilp)
	{
		mAnimationClip = animationCilp;
		mBlendAnimationClip = nullptr;
		link();
	}
	inline void BoneHierarchyCache::SetAnimation(std::shared_ptr<fq::common::AnimationClip> animationCilp, std::shared_ptr<fq::common::AnimationClip> blendAnimationClip)
	{
		mAnimationClip = animationCilp;
		mBlendAnimationClip = blendAnimationClip;
		linkBlendAnim();
	}

	inline const std::shared_ptr<BoneHierarchy>& BoneHierarchyCache::GetBoneHierarchy() const
	{
		return mBoneHierarchy;
	}
	inline const std::shared_ptr<fq::common::AnimationClip>& BoneHierarchyCache::GetAnimationClip() const
	{
		return mAnimationClip;
	}
	inline const std::vector<DirectX::SimpleMath::Matrix>& BoneHierarchyCache::GetTransposedFinalTransforms() const
	{
		return mTransposedFinalTransforms;
	}

	inline const DirectX::SimpleMath::Matrix& BoneHierarchyCache::GetRootTransform(const std::string& boneName) const
	{
		unsigned int index = mBoneHierarchy->GetBoneIndex(boneName);
		return mRootTransforms[index];
	}
	inline bool BoneHierarchyCache::TryGetRootTransform(const std::string& boneName, DirectX::SimpleMath::Matrix* outRootTransform)
	{
		unsigned int index;

		if (mBoneHierarchy->TryGetBoneIndex(boneName, &index))
		{
			*outRootTransform = mRootTransforms[index];

			return true;
		}

		return false;
	}
	inline const DirectX::SimpleMath::Matrix& BoneHierarchyCache::GetRootTransform(size_t index) const
	{
		return mRootTransforms[index];
	}
	inline const std::vector<DirectX::SimpleMath::Matrix>& BoneHierarchyCache::GetRootTransforms() const
	{
		return mRootTransforms;
	}

	inline bool BoneHierarchyCache::checkValid() const
	{
		return mBoneHierarchy != nullptr && mAnimationClip != nullptr;
	}
}
