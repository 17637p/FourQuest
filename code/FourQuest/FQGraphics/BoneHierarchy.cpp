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

			Bone bone;
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

	BoneHierarchyCache::BoneHierarchyCache(std::shared_ptr<BoneHierarchy> boneHierarchy)
		: mBoneHierarchy(boneHierarchy)
	{
		mFinalTransforms.resize(mBoneHierarchy->GetBoneCount(), DirectX::SimpleMath::Matrix::Identity);
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

		// clear하는 함수 없나
		for (auto& transform : mFinalTransforms)
		{
			transform = DirectX::SimpleMath::Matrix::Identity;
		}

		for (const auto& [bone, nodeClip] : mBoneNodeClipCache)
		{
			AnimationHelper::FindKeyframe(nodeClip.Keyframes, *mAnimationClip, timePos, &lhs, &rhs, &weight);
			fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			mFinalTransforms[bone.Index] = AnimationHelper::CreateMatrix(keyframe);
		}

		const std::vector<Bone>& bones = mBoneHierarchy->GetBones();
		for (size_t i = 1; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mFinalTransforms[i] = mFinalTransforms[i] * mFinalTransforms[bones[i].ParentIndex];
		}

		for (size_t i = 0; i < mBoneHierarchy->GetBoneCount(); ++i)
		{
			assert(bones[i].Index == i);
			mFinalTransforms[i] = bones[i].OffsetMatrix * mFinalTransforms[i];
			mFinalTransforms[i] = mFinalTransforms[i].Transpose();
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

		std::map<std::string, const struct Bone&> boneMap;
		std::map<std::string, const struct fq::common::NodeClip&> nodeClipMap;

		for (const Bone& bone : mBoneHierarchy->GetBones())
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
				mBoneNodeClipCache.push_back({ bone, find->second });
			}
		}
	}
}