#include "BoneHierarchy.h"

namespace fq::graphics
{
	BoneHierarchy::BoneHierarchy(fq::common::Model model)
	{
		std::vector<DirectX::SimpleMath::Matrix> toRoot;
		toRoot.reserve(model.Meshes.size());

		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& node = nodeMeshPair.first;

			fq::common::Bone bone;
			bone.Name = node.Name;
			bone.Index = node.Index;
			bone.ParentIndex = node.ParentIndex;

			toRoot.push_back(node.ToParentMatrix);

			if (node.ParentIndex != fq::common::Node::INVALID_INDEX)
			{
				toRoot[node.Index] = toRoot[node.Index] * toRoot[node.ParentIndex];
			}

			bone.OffsetMatrix = toRoot[node.Index].Invert();

			mBones.push_back(std::move(bone));
		}
	}

	unsigned int BoneHierarchy::GetBoneIndex(const std::string& boneName) const
	{
		for (const auto& bone : mBones)
		{
			if (bone.Name == boneName)
			{
				return bone.Index;
			}
		}

		assert(false);
	}

	bool BoneHierarchy::TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex)
	{
		for (const auto& bone : mBones)
		{
			if (bone.Name == boneName)
			{
				*outBoneIndex = bone.Index;
				return true;
			}
		}

		return false;
	}

	BoneHierarchyCache::BoneHierarchyCache(std::shared_ptr<BoneHierarchy> boneHierarchy)
		: mBoneHierarchy(boneHierarchy)
	{
		mRootTransforms.resize(mBoneHierarchy->GetBoneCount(), DirectX::SimpleMath::Matrix::Identity);
		mTransposedFinalTransforms.resize(mBoneHierarchy->GetBoneCount(), DirectX::SimpleMath::Matrix::Identity);
	}

	void BoneHierarchyCache::Clear()
	{
		assert(mRootTransforms.size() == mTransposedFinalTransforms.size());

		for (size_t i = 0; i < mRootTransforms.size(); ++i)
		{
			mRootTransforms[i] = DirectX::SimpleMath::Matrix::Identity;
			mTransposedFinalTransforms[i] = DirectX::SimpleMath::Matrix::Identity;
		}
	}

	void BoneHierarchyCache::Update(float timePos)
	{
		if (!checkValid())
		{
			return;
		}

		fq::common::Keyframe lhs;
		fq::common::Keyframe rhs;
		float weight;

		Clear();

		for (size_t i = 0; i < mBoneNodeClipCache.size(); ++i)
		{
			const auto& [bone, nodeClip] = mBoneNodeClipCache[i];

			if (nodeClip != nullptr)
			{
				AnimationHelper::FindKeyframe(nodeClip->Keyframes, *mAnimationClip, timePos, &lhs, &rhs, &weight);
				fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
				mRootTransforms[bone->Index] = AnimationHelper::CreateMatrix(keyframe);
			}
		}

		const std::vector<fq::common::Bone>& bones = mBoneHierarchy->GetBones();
		for (size_t i = 1; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mRootTransforms[i] = mRootTransforms[i] * mRootTransforms[bones[i].ParentIndex];
		}

		for (size_t i = 0; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mTransposedFinalTransforms[i] = bones[i].OffsetMatrix * mRootTransforms[i];
			mTransposedFinalTransforms[i] = mTransposedFinalTransforms[i].Transpose();
		}
	}

	void BoneHierarchyCache::Update(float timePos, float blendTimePos, float blendWeight)
	{
		if (!checkValid() || mBlendAnimationClip == nullptr)
		{
			return;
		}

		fq::common::Keyframe lhs;
		fq::common::Keyframe rhs;
		float weight;

		Clear();

		for (size_t i = 0; i < mBoneNodeClipCache.size(); ++i)
		{
			const auto& [bone, nodeClip] = mBoneNodeClipCache[i];
			const auto& [blendBone, blendNodeClip] = mBlendBoneNodeClipCache[i];

			fq::common::Keyframe keyframe;
			fq::common::Keyframe blendKeyframe;

			if (nodeClip != nullptr)
			{
				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;
				AnimationHelper::FindKeyframe(nodeClip->Keyframes, *mAnimationClip, timePos, &lhs, &rhs, &weight);
				keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			if (blendNodeClip != nullptr)
			{
				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;
				AnimationHelper::FindKeyframe(blendNodeClip->Keyframes, *mBlendAnimationClip, blendTimePos, &lhs, &rhs, &weight);
				blendKeyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			fq::common::Keyframe nodeKeyframe = AnimationHelper::Interpolate(keyframe, blendKeyframe, blendWeight);
			mTransposedFinalTransforms[bone->Index] = AnimationHelper::CreateMatrix(nodeKeyframe);
		}

		const std::vector<fq::common::Bone>& bones = mBoneHierarchy->GetBones();
		for (size_t i = 1; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mTransposedFinalTransforms[i] = mTransposedFinalTransforms[i] * mTransposedFinalTransforms[bones[i].ParentIndex];
		}

		for (size_t i = 0; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mTransposedFinalTransforms[i] = bones[i].OffsetMatrix * mTransposedFinalTransforms[i];
			mTransposedFinalTransforms[i] = mTransposedFinalTransforms[i].Transpose();
		}
	}

	void BoneHierarchyCache::link()
	{
		if (!checkValid())
		{
			return;
		}

		mBoneNodeClipCache.clear();
		mBoneNodeClipCache.reserve(mBoneHierarchy->GetBoneCount());

		std::map<std::string, const struct fq::common::Bone&> boneMap;
		std::map<std::string, const struct fq::common::NodeClip&> nodeClipMap;

		for (const fq::common::Bone& bone : mBoneHierarchy->GetBones())
		{
			boneMap.insert({ bone.Name, bone });
		}

		for (const fq::common::NodeClip& nodeClip : mAnimationClip->NodeClips)
		{
			nodeClipMap.insert({ nodeClip.NodeName, nodeClip });
		}

		for (const auto& [name, bone] : boneMap)
		{
			auto find = nodeClipMap.find(name);

			if (find != nodeClipMap.end())
			{
				mBoneNodeClipCache.push_back({ &bone, &find->second });
			}
			else
			{
				mBoneNodeClipCache.push_back({ &bone, nullptr });
			}
		}

		mBlendAnimationClip = nullptr;
		mBlendBoneNodeClipCache.clear();
	}

	void BoneHierarchyCache::linkBlendAnim()
	{
		if (!checkValid() && mBlendAnimationClip == nullptr)
		{
			return;
		}

		mBoneNodeClipCache.clear();
		mBoneNodeClipCache.reserve(mBoneHierarchy->GetBoneCount());

		std::map<std::string, const struct fq::common::Bone&> boneMap;

		for (const fq::common::Bone& bone : mBoneHierarchy->GetBones())
		{
			boneMap.insert({ bone.Name, bone });
		}

		{
			std::map<std::string, const struct fq::common::NodeClip&> nodeClipMap;

			for (const fq::common::NodeClip& nodeClip : mAnimationClip->NodeClips)
			{
				nodeClipMap.insert({ nodeClip.NodeName, nodeClip });
			}

			for (const auto& [name, bone] : boneMap)
			{
				auto find = nodeClipMap.find(name);

				if (find != nodeClipMap.end())
				{
					mBoneNodeClipCache.push_back({ &bone, &find->second });
				}
				else
				{
					mBoneNodeClipCache.push_back({ &bone, nullptr });
				}
			}
		}

		mBlendBoneNodeClipCache.clear();
		mBlendBoneNodeClipCache.reserve(mBoneHierarchy->GetBoneCount());

		{
			std::map<std::string, const struct fq::common::NodeClip&> nodeClipMap;

			for (const fq::common::NodeClip& nodeClip : mBlendAnimationClip->NodeClips)
			{
				nodeClipMap.insert({ nodeClip.NodeName, nodeClip });
			}

			for (const auto& [name, bone] : boneMap)
			{
				auto find = nodeClipMap.find(name);

				if (find != nodeClipMap.end())
				{
					mBlendBoneNodeClipCache.push_back({ &bone, &find->second });
				}
				else
				{
					mBlendBoneNodeClipCache.push_back({ &bone, nullptr });
				}
			}
		}
	}
}