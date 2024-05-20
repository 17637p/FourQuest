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
	struct Bone
	{
		std::string Name;
		unsigned int Index;
		unsigned int ParentIndex;
		DirectX::SimpleMath::Matrix OffsetMatrix;
	};

	class BoneHierarchy
	{
	public:
		BoneHierarchy(fq::common::Model model);
		~BoneHierarchy() = default;

		inline const std::vector<Bone>& GetBones() const;
		inline size_t GetBoneCount() const;

	private:
		std::vector<Bone> mBones;
	};

	inline const std::vector<Bone>& BoneHierarchy::GetBones() const
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
		using BoneNodeClipCache = std::pair<const struct Bone*, const struct fq::common::NodeClip*>;

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
		inline const std::vector<DirectX::SimpleMath::Matrix>& GetFinalTransforms() const;
		// 캐싱 데이터의 getter는 필요할까?

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
		std::vector<DirectX::SimpleMath::Matrix> mFinalTransforms;
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
	inline const std::vector<DirectX::SimpleMath::Matrix>& BoneHierarchyCache::GetFinalTransforms() const
	{
		return mFinalTransforms;
	}

	inline bool BoneHierarchyCache::checkValid() const
	{
		return mBoneHierarchy != nullptr && mAnimationClip != nullptr;
	}
}
