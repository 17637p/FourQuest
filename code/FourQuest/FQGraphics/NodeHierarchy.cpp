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
		mRootTransforms.resize(boneCount, DirectX::SimpleMath::Matrix::Identity);
		mTransposedFinalTransforms.resize(boneCount, DirectX::SimpleMath::Matrix::Identity);
	}

	void NodeHierarchyInstance::SetBindPose()
	{
		std::fill(mRootTransforms.begin(), mRootTransforms.end(), DirectX::SimpleMath::Matrix::Identity);
		std::fill(mTransposedFinalTransforms.begin(), mTransposedFinalTransforms.end(), DirectX::SimpleMath::Matrix::Identity);
	}

	void NodeHierarchyInstance::Update(float timePos, std::shared_ptr<IAnimation> animation)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& find = animationCache.find(animation);
		const auto& animClip = animation->GetAnimationClip();

		SetBindPose();

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
				mRootTransforms[bone->Index] = AnimationHelper::CreateMatrix(keyframe);
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
				mRootTransforms[findedBone->second.Index] = AnimationHelper::CreateMatrix(keyframe);
			}
		}

		calculateRootTransform();
	}
	void NodeHierarchyInstance::Update(float lhsTimePos, std::shared_ptr<IAnimation> lhsAnimation, float rhsTimePos, std::shared_ptr<IAnimation> rhsAnimation, float weight)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& lhsFind = animationCache.find(lhsAnimation);
		const auto& rhsFind = animationCache.find(rhsAnimation);

		if (lhsFind == animationCache.end() || rhsFind == animationCache.end())
		{
			return;
		}

		SetBindPose();

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
			fq::common::Keyframe lhs;
			fq::common::Keyframe rhs;
			float weight;

			if (lhsNodeClip != nullptr)
			{
				AnimationHelper::FindKeyframe(lhsNodeClip->Keyframes, lhsAnimationClip, lhsTimePos, &lhs, &rhs, &weight);
				lhsKeyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			if (rhsNodeClip != nullptr)
			{
				AnimationHelper::FindKeyframe(rhsNodeClip->Keyframes, rhsAnimationClip, rhsTimePos, &lhs, &rhs, &weight);
				rhsKeyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
			}

			fq::common::Keyframe nodeKeyframe = AnimationHelper::Interpolate(lhsKeyframe, rhsKeyframe, weight);
			mRootTransforms[i] = AnimationHelper::CreateMatrix(nodeKeyframe);
		}

		calculateRootTransform();
	}

	void NodeHierarchyInstance::calculateRootTransform()
	{
		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();
		for (size_t i = 1; i < bones.size(); ++i)
		{
			assert(bones[i].Index == i);
			mRootTransforms[i] = mRootTransforms[i] * mRootTransforms[bones[i].ParentIndex];
		}
		for (size_t i = 0; i < bones.size(); ++i)
		{
			assert(bones[i].Index == i);
			mTransposedFinalTransforms[i] = (bones[i].OffsetMatrix * mRootTransforms[i]).Transpose();
		}
	}
}