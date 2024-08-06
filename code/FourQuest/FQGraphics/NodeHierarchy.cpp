#include "NodeHierarchy.h"

namespace fq::graphics
{
	NodeHierarchy::NodeHierarchy(const fq::common::Model& model)
	{
		Create(model);
	}

	NodeHierarchy::NodeHierarchy(const std::vector<fq::common::Node> nodes)
	{
		Create(nodes);
	}

	void NodeHierarchy::Create(const fq::common::Model& model)
	{
		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& node = nodeMeshPair.first;

			fq::common::Bone bone;
			bone.Name = node.Name;
			bone.Index = node.Index;
			bone.ParentIndex = node.ParentIndex;
			bone.ToParentMatrix = node.ToParentMatrix;
			bone.OffsetMatrix = node.OffsetMatrix;

			mBones.push_back(std::move(bone));
		}
	}
	void NodeHierarchy::Create(const std::vector<fq::common::Node> nodes)
	{
		for (const auto& node : nodes)
		{
			fq::common::Bone bone;
			bone.Name = node.Name;
			bone.Index = node.Index;
			bone.ParentIndex = node.ParentIndex;
			bone.ToParentMatrix = node.ToParentMatrix;
			bone.OffsetMatrix = node.OffsetMatrix;

			mBones.push_back(std::move(bone));
		}
	}

	void NodeHierarchy::RegisterAnimation(std::shared_ptr<IAnimation> animation)
	{
		assert(animation != nullptr);
		auto find = mAnimationCaches.find(animation);

		if (find != mAnimationCaches.end())
		{
			return;
		}

		std::vector<BoneNodeClipCache> cacheData;
		cacheData.reserve(mBones.size());

		std::map<std::string, const struct fq::common::Bone&> boneMap;
		std::map<std::string, const struct fq::common::NodeClip&> nodeClipMap;

		for (const fq::common::Bone& bone : mBones)
		{
			boneMap.insert({ bone.Name, bone });
		}

		for (const fq::common::NodeClip& nodeClip : animation->GetAnimationClip().NodeClips)
		{
			nodeClipMap.insert({ nodeClip.NodeName, nodeClip });
		}

		for (const auto& [name, bone] : boneMap)
		{
			auto find = nodeClipMap.find(name);

			if (find != nodeClipMap.end())
			{
				cacheData.push_back({ &bone, &find->second });
			}
			else
			{
				cacheData.push_back({ &bone, nullptr });
			}
		}

		mAnimationCaches.insert({ animation, cacheData });
		mRegisterAnimations.insert(animation);
	}

	std::shared_ptr<INodeHierarchyInstance> NodeHierarchy::CreateNodeHierarchyInstance()
	{
		return std::make_shared<NodeHierarchyInstance>(shared_from_this());
	}

	unsigned int NodeHierarchy::GetBoneIndex(const std::string& boneName) const
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

	bool NodeHierarchy::TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex) const
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

	NodeHierarchyInstance::NodeHierarchyInstance(std::shared_ptr<INodeHierarchy> nodeHierarchy)
		: mNodeHierarchy(nodeHierarchy)
	{
		size_t boneCount = mNodeHierarchy->GetBones().size();
		mLocalTransforms.resize(boneCount, DirectX::SimpleMath::Matrix::Identity);
		mRootTransforms.resize(boneCount, DirectX::SimpleMath::Matrix::Identity);
		mTransposedFinalTransforms.resize(boneCount, DirectX::SimpleMath::Matrix::Identity);
		SetBindPose();
	}

	void NodeHierarchyInstance::SetBindPose()
	{
		setBindPoseLocal();
		calculateRootTransform();
	}

	void NodeHierarchyInstance::Update(float timePos, const std::shared_ptr<IAnimation>& animation)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& find = animationCache.find(animation);
		const auto& animClip = animation->GetAnimationClip();

		setBindPoseLocal();

		if (find != animationCache.end())
		{
			const auto boneNodeClipCache = find->second;

			for (const auto& [bone, nodeClip] : boneNodeClipCache)
			{
				if (nodeClip == nullptr)
				{
					continue;
				}

				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;

				AnimationHelper::FindKeyframe(nodeClip->Keyframes, animClip, timePos, &lhs, &rhs, &weight);
				fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
				mLocalTransforms[bone->Index] = AnimationHelper::CreateMatrix(keyframe);
			}
		}
		else
		{
			const auto& bones = mNodeHierarchy->GetBones();
			std::map<std::string, const struct fq::common::Bone&> boneMap;

			for (const fq::common::Bone& bone : bones)
			{
				boneMap.insert({ bone.Name, bone });
			}

			for (const auto& nodeClip : animClip.NodeClips)
			{
				auto findedBone = boneMap.find(nodeClip.NodeName);

				if (findedBone == boneMap.end())
				{
					continue;
				}

				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;

				AnimationHelper::FindKeyframe(nodeClip.Keyframes, animClip, timePos, &lhs, &rhs, &weight);
				fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
				mLocalTransforms[findedBone->second.Index] = AnimationHelper::CreateMatrix(keyframe);
			}
		}

		calculateRootTransform();
	}
	void NodeHierarchyInstance::Update(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& lhsFind = animationCache.find(lhsAnimation);
		const auto& rhsFind = animationCache.find(rhsAnimation);

		if (lhsFind == animationCache.end() || rhsFind == animationCache.end())
		{
			return;
		}

		setBindPoseLocal();

		const size_t BONE_COUNT = mNodeHierarchy->GetBones().size();

		const auto& lhsBoneNodeClipCache = lhsFind->second;
		const auto& rhsBoneNodeClipCache = rhsFind->second;
		const auto& lhsAnimationClip = lhsAnimation->GetAnimationClip();
		const auto& rhsAnimationClip = rhsAnimation->GetAnimationClip();

		for (size_t i = 0; i < BONE_COUNT; ++i)
		{
			const auto& [lhsBone, lhsNodeClip] = lhsBoneNodeClipCache[i];
			const auto& [rhsBone, rhsNodeClip] = rhsBoneNodeClipCache[i];

			fq::common::Keyframe lhsKeyframe;
			fq::common::Keyframe rhsKeyframe;

			if (lhsNodeClip != nullptr)
			{
				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;
				AnimationHelper::FindKeyframe(lhsNodeClip->Keyframes, lhsAnimationClip, lhsTimePos, &lhs, &rhs, &weight);
				lhsKeyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			if (rhsNodeClip != nullptr)
			{
				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;
				AnimationHelper::FindKeyframe(rhsNodeClip->Keyframes, rhsAnimationClip, rhsTimePos, &lhs, &rhs, &weight);
				rhsKeyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			fq::common::Keyframe nodeKeyframe = AnimationHelper::Interpolate(lhsKeyframe, rhsKeyframe, weight);
			mLocalTransforms[lhsBone->Index] = AnimationHelper::CreateMatrix(nodeKeyframe);
		}

		calculateRootTransform();
	}

	void NodeHierarchyInstance::UpdateByLocalTransform()
	{
		calculateRootTransform();
	}

	void NodeHierarchyInstance::UpdateByLocalTransform(float timePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& rhsFind = animationCache.find(rhsAnimation);

		if (rhsFind == animationCache.end())
		{
			return;
		}

		setBindPoseLocal();

		const size_t BONE_COUNT = mNodeHierarchy->GetBones().size();

		const auto& rhsBoneNodeClipCache = rhsFind->second;
		const auto& rhsAnimationClip = rhsAnimation->GetAnimationClip();

		for (size_t i = 0; i < BONE_COUNT; ++i)
		{
			const auto& [rhsBone, rhsNodeClip] = rhsBoneNodeClipCache[i];

			if (rhsNodeClip == nullptr)
			{
				continue;
			}

			fq::common::Keyframe lhsKeyframe;
			fq::common::Keyframe rhsKeyframe;

			mLocalTransforms[rhsBone->Index].Decompose(lhsKeyframe.Scale, lhsKeyframe.Rotation, lhsKeyframe.Translation);

			{
				fq::common::Keyframe localLhs;
				fq::common::Keyframe localRhs;
				float localWeight;
				AnimationHelper::FindKeyframe(rhsNodeClip->Keyframes, rhsAnimationClip, timePos, &localLhs, &localRhs, &localWeight);
				rhsKeyframe = AnimationHelper::Interpolate(localLhs, localRhs, localWeight);
			}

			fq::common::Keyframe nodeKeyframe = AnimationHelper::Interpolate(lhsKeyframe, rhsKeyframe, weight);
			mLocalTransforms[rhsBone->Index] = AnimationHelper::CreateMatrix(nodeKeyframe);
		}

		calculateRootTransform();
	}

	void NodeHierarchyInstance::SetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform)
	{
		assert(index < mLocalTransforms.size());
		mLocalTransforms[index] = transform;
	}

	bool NodeHierarchyInstance::TrySetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform)
	{
		if (index >= mLocalTransforms.size())
		{
			return false;
		}

		mLocalTransforms[index] = transform;

		return true;
	}

	void NodeHierarchyInstance::clear()
	{
		std::fill(mLocalTransforms.begin(), mLocalTransforms.end(), DirectX::SimpleMath::Matrix::Identity);
		std::fill(mRootTransforms.begin(), mRootTransforms.end(), DirectX::SimpleMath::Matrix::Identity);
		std::fill(mTransposedFinalTransforms.begin(), mTransposedFinalTransforms.end(), DirectX::SimpleMath::Matrix::Identity);
	}

	void NodeHierarchyInstance::calculateRootTransform()
	{
		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();

		mRootTransforms[0] = mLocalTransforms[0];

		for (size_t i = 1; i < bones.size(); ++i)
		{
			assert(bones[i].Index == i);
			mRootTransforms[i] = mLocalTransforms[i] * mRootTransforms[bones[i].ParentIndex];
		}
		for (size_t i = 0; i < bones.size(); ++i)
		{
			assert(bones[i].Index == i);
			mTransposedFinalTransforms[i] = (bones[i].OffsetMatrix * mRootTransforms[i]).Transpose();
		}
	}

	void NodeHierarchyInstance::setBindPoseLocal()
	{
		const auto& bones = mNodeHierarchy->GetBones();

		for (const auto& bone : bones)
		{
			mLocalTransforms[bone.Index] = bone.ToParentMatrix;
		}
	}
}