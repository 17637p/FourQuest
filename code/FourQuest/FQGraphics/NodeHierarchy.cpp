#include "NodeHierarchy.h"
#include "D3D11Texture.h"

namespace fq::graphics
{
	NodeHierarchy::NodeHierarchy(const fq::common::Model& model)
	{
		std::vector<fq::common::Node> nodes;
		nodes.reserve(model.Meshes.size());

		for (const auto& nodeMeshPair : model.Meshes)
		{
			const auto& node = nodeMeshPair.first;
			nodes.push_back(node);
		}

		Create(nodes);
	}

	NodeHierarchy::NodeHierarchy(const std::vector<fq::common::Node>& nodes)
	{
		Create(nodes);
	}

	void NodeHierarchy::Create(const std::vector<fq::common::Node>& nodes)
	{
		// spine을 포함한 이름이 없을 경우 1번인덱스를 스파인이라 가정함
		mSpineIndex = 1;
		bool bIsFind = false;

		for (const auto& node : nodes)
		{
			fq::common::Bone bone;
			bone.Name = node.Name;
			bone.Index = node.Index;
			bone.ParentIndex = node.ParentIndex;
			bone.ToParentMatrix = node.ToParentMatrix;
			bone.OffsetMatrix = node.OffsetMatrix;

			if (bone.ParentIndex != fq::common::Node::INVALID_INDEX)
			{
				mBones[bone.ParentIndex].ChildIndices.push_back(bone.Index);
			}

			if (!bIsFind)
			{
				std::string lowerBoneName;
				lowerBoneName.resize(bone.Name.size());

				for (size_t i = 0; i < bone.Name.size(); ++i)
				{
					lowerBoneName[i] = std::tolower(bone.Name[i]);
				}

				if (lowerBoneName.find("spine") != lowerBoneName.npos)
				{
					mSpineIndex = bone.Index;
					bIsFind = true;
				}
			}

			mBones.push_back(std::move(bone));
		}

		if (mBones.size() > mSpineIndex)
		{
			SetSpineBoneIndex(mSpineIndex);
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

	void NodeHierarchy::CreateAnimationTexture(const std::shared_ptr<class D3D11Device>& device)
	{
		size_t index = 0;
		mAnimationKeyframes.resize(mRegisterAnimations.size());

		std::map<std::string, const fq::common::Bone&> boneMap;

		for (const fq::common::Bone& bone : mBones)
		{
			boneMap.insert({ bone.Name, bone });
		}

		for (auto iter = mRegisterAnimations.begin(); iter != mRegisterAnimations.end(); ++iter, ++index)
		{
			std::shared_ptr<IAnimation> animationInterface = *iter;

			mRegisterAnimationIndexMap.insert({ animationInterface, index });

			// gen data
			const auto& animationClip = animationInterface->GetAnimationClip();

			for (size_t i = 0; i < std::min<size_t>(animationClip.FrameCount, AnimationKeyFrames::MAX_MODEL_KEYFRAMES); ++i)
			{
				std::vector<DirectX::SimpleMath::Matrix> localTransform(AnimationKeyFrames::MAX_MODEL_TRANSFORMS, DirectX::SimpleMath::Matrix::Identity);
				std::vector<DirectX::SimpleMath::Matrix> rootTransform(AnimationKeyFrames::MAX_MODEL_TRANSFORMS);

				for (const auto& bone : mBones)
				{
					localTransform[bone.Index] = bone.ToParentMatrix;
				}

				for (const auto& nodeClip : animationClip.NodeClips)
				{
					auto findedBone = boneMap.find(nodeClip.NodeName);

					if (findedBone == boneMap.end())
					{
						continue;
					}

					localTransform[findedBone->second.Index] = AnimationHelper::CreateMatrix(nodeClip.Keyframes[i]);
				}

				rootTransform[0] = localTransform[0];

				for (size_t j = 1; j < mBones.size(); ++j)
				{
					assert(mBones[j].Index == j);
					rootTransform[j] = localTransform[j] * rootTransform[mBones[j].ParentIndex];
				}
				for (size_t j = 0; j < mBones.size(); ++j)
				{
					assert(mBones[j].Index == j);
					rootTransform[j] = mBones[j].OffsetMatrix * rootTransform[j];
				}

				for (size_t j = 0; j < AnimationKeyFrames::MAX_MODEL_TRANSFORMS; ++j)
				{
					mAnimationKeyframes[index].KeyFrames[i][j] = rootTransform[j];
				}
			}
		}

		mAnimationKeyframeTexture = std::make_shared<D3D11TextureArray>(device, mAnimationKeyframes);
	}

	void NodeHierarchy::ReleaseAnimationTexture()
	{
		mAnimationKeyframes.clear();
		mAnimationKeyframeTexture = nullptr;
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

		return 0;
		//assert(false);
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
	void NodeHierarchy::SetSpineBoneName(const std::string& boneName)
	{
		unsigned int boneIndex = GetBoneIndex(boneName);
		SetSpineBoneIndex(boneIndex);
	}
	void NodeHierarchy::SetSpineBoneIndex(unsigned int boneIndex)
	{
		if (mBones[boneIndex].ChildIndices.size() == 2)
		{
			mUpperBodyEndIndex = mBones[boneIndex].ChildIndices[1] - 1;
			mLowerBodyStartIndex = mBones[boneIndex].ChildIndices[1];
		}

		// 만약 스파인에 들어간 넘이 2개가 아니라면 어떻게 할 거임?
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

		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();
		calculateRootTransform(0, bones.size() - 1);
	}

	void NodeHierarchyInstance::SetBindPoseLocalTransform()
	{
		setBindPoseLocal();
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
			const auto& boneNodeClipCache = find->second;

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

		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();
		calculateRootTransform(0, bones.size() - 1);
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

		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();
		calculateRootTransform(0, bones.size() - 1);
	}

	void NodeHierarchyInstance::UpdateGPUData(float timePos, const std::shared_ptr<IAnimation>& animation)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& find = animationCache.find(animation);
		const auto& animClip = animation->GetAnimationClip();

		const auto& boneNodeClipCache = find->second;
		const auto& registerAnimationIndexMap = nodeHierarchy->GetRegisterAnimationIndexMap();
		auto findedIndex = registerAnimationIndexMap.find(animation);

		// 인스턴싱 가능
		if (nodeHierarchy->GetIsCreatedAnimationTexture()
			&& findedIndex != registerAnimationIndexMap.end())
		{
			for (const auto& [bone, nodeClip] : boneNodeClipCache)
			{
				if (nodeClip != nullptr)
				{
					mTweenDesc.ClearNextAnim();

					AnimationHelper::FindKeyframeIndex(nodeClip->Keyframes, animClip, timePos, &mTweenDesc.curr.currFrame, &mTweenDesc.curr.nextFrame, &mTweenDesc.curr.ratio);
					mTweenDesc.curr.animIndex = findedIndex->second;
					break;
				}
			}
		}
	}

	void NodeHierarchyInstance::UpdateGPUData(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& lhsFind = animationCache.find(lhsAnimation);
		const auto& rhsFind = animationCache.find(rhsAnimation);

		if (lhsFind == animationCache.end() || rhsFind == animationCache.end())
		{
			return;
		}

		const auto& registerAnimationIndexMap = nodeHierarchy->GetRegisterAnimationIndexMap();
		auto findedLhsIndex = registerAnimationIndexMap.find(lhsAnimation);
		auto findedRhsIndex = registerAnimationIndexMap.find(rhsAnimation);

		// 인스턴싱 여부
		if (nodeHierarchy->GetIsCreatedAnimationTexture()
			&& findedLhsIndex != registerAnimationIndexMap.end()
			&& findedRhsIndex != registerAnimationIndexMap.end())
		{
			mTweenDesc.tweenRatio = weight;

			const size_t BONE_COUNT = mNodeHierarchy->GetBones().size();

			const auto& lhsBoneNodeClipCache = lhsFind->second;
			const auto& rhsBoneNodeClipCache = rhsFind->second;
			const auto& lhsAnimationClip = lhsAnimation->GetAnimationClip();
			const auto& rhsAnimationClip = rhsAnimation->GetAnimationClip();

			bool bFindLhsNode = false;
			bool bFindRhsNode = false;

			for (size_t i = 0; i < BONE_COUNT; ++i)
			{
				if (bFindLhsNode && bFindRhsNode)
				{
					break;
				}

				const auto& [lhsBone, lhsNodeClip] = lhsBoneNodeClipCache[i];
				const auto& [rhsBone, rhsNodeClip] = rhsBoneNodeClipCache[i];

				if (lhsNodeClip != nullptr)
				{
					bFindLhsNode = true;

					AnimationHelper::FindKeyframeIndex(lhsNodeClip->Keyframes, lhsAnimationClip, lhsTimePos, &mTweenDesc.curr.currFrame, &mTweenDesc.curr.nextFrame, &mTweenDesc.curr.ratio);
					mTweenDesc.curr.animIndex = findedLhsIndex->second;
				}
				if (rhsNodeClip != nullptr)
				{
					bFindRhsNode = true;

					AnimationHelper::FindKeyframeIndex(rhsNodeClip->Keyframes, rhsAnimationClip, rhsTimePos, &mTweenDesc.next.currFrame, &mTweenDesc.next.nextFrame, &mTweenDesc.next.ratio);
					mTweenDesc.next.animIndex = findedLhsIndex->second;
				}
			}
		}
	}

	void NodeHierarchyInstance::UpdateByLocalTransform()
	{
		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();
		calculateRootTransform(0, bones.size() - 1);
	}

	void NodeHierarchyInstance::UpdateByLocalTransform(unsigned int startIndex, unsigned int endIndex)
	{
		calculateRootTransform(startIndex, endIndex);
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

		const auto& bones = mNodeHierarchy->GetBones();
		std::vector <DirectX::SimpleMath::Matrix> bindPoseTransform(bones.size());
		for (const auto& bone : bones)
		{
			bindPoseTransform[bone.Index] = bone.ToParentMatrix;
		}

		const size_t BONE_COUNT = mNodeHierarchy->GetBones().size();

		const auto& rhsBoneNodeClipCache = rhsFind->second;
		const auto& rhsAnimationClip = rhsAnimation->GetAnimationClip();

		for (size_t i = 0; i < BONE_COUNT; ++i)
		{
			const auto& [rhsBone, rhsNodeClip] = rhsBoneNodeClipCache[i];

			fq::common::Keyframe lhsKeyframe;
			fq::common::Keyframe rhsKeyframe;

			mLocalTransforms[rhsBone->Index].Decompose(lhsKeyframe.Scale, lhsKeyframe.Rotation, lhsKeyframe.Translation);

			if (rhsNodeClip != nullptr)
			{
				fq::common::Keyframe localLhs;
				fq::common::Keyframe localRhs;
				float localWeight;
				AnimationHelper::FindKeyframe(rhsNodeClip->Keyframes, rhsAnimationClip, timePos, &localLhs, &localRhs, &localWeight);
				rhsKeyframe = AnimationHelper::Interpolate(localLhs, localRhs, localWeight);
			}
			else
			{
				bindPoseTransform[rhsBone->Index].Decompose(rhsKeyframe.Scale, rhsKeyframe.Rotation, rhsKeyframe.Translation);
			}

			fq::common::Keyframe nodeKeyframe = AnimationHelper::Interpolate(lhsKeyframe, rhsKeyframe, weight);
			mLocalTransforms[rhsBone->Index] = AnimationHelper::CreateMatrix(nodeKeyframe);
		}

		calculateRootTransform(0, bones.size() - 1);
	}

	void NodeHierarchyInstance::UpdateLocalTransformRange(float timePos, const std::shared_ptr<IAnimation>& animation, unsigned int startIndex, unsigned int endIndex)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& find = animationCache.find(animation);
		const auto& animClip = animation->GetAnimationClip();

		if (find != animationCache.end())
		{
			const auto& boneNodeClipCache = find->second;

			for (const auto& [bone, nodeClip] : boneNodeClipCache)
			{
				unsigned int boneIndex = bone->Index;

				if (boneIndex < startIndex || boneIndex > endIndex)
				{
					continue;
				}
				if (nodeClip == nullptr)
				{
					continue;
				}

				fq::common::Keyframe lhs;
				fq::common::Keyframe rhs;
				float weight;

				AnimationHelper::FindKeyframe(nodeClip->Keyframes, animClip, timePos, &lhs, &rhs, &weight);
				fq::common::Keyframe keyframe = AnimationHelper::Interpolate(lhs, rhs, weight);
				mLocalTransforms[boneIndex] = AnimationHelper::CreateMatrix(keyframe);
			}
		}
		else
		{
			const auto& bones = mNodeHierarchy->GetBones();
			std::map<std::string, const struct fq::common::Bone&> boneMap;

			for (const fq::common::Bone& bone : bones)
			{
				unsigned int boneIndex = bone.Index;

				if (boneIndex < startIndex || boneIndex > endIndex)
				{
					continue;
				}

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
	}
	void NodeHierarchyInstance::UpdateLocalTransformRange(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight, unsigned int startIndex, unsigned int endIndex)
	{
		std::shared_ptr<NodeHierarchy> nodeHierarchy = std::static_pointer_cast<NodeHierarchy>(mNodeHierarchy);
		const auto& animationCache = nodeHierarchy->GetAnimationCaches();
		const auto& lhsFind = animationCache.find(lhsAnimation);
		const auto& rhsFind = animationCache.find(rhsAnimation);

		if (lhsFind == animationCache.end() || rhsFind == animationCache.end())
		{
			return;
		}

		const size_t BONE_COUNT = mNodeHierarchy->GetBones().size();

		const auto& lhsBoneNodeClipCache = lhsFind->second;
		const auto& rhsBoneNodeClipCache = rhsFind->second;
		const auto& lhsAnimationClip = lhsAnimation->GetAnimationClip();
		const auto& rhsAnimationClip = rhsAnimation->GetAnimationClip();

		for (size_t i = 0; i < BONE_COUNT; ++i)
		{
			const auto& [lhsBone, lhsNodeClip] = lhsBoneNodeClipCache[i];
			const auto& [rhsBone, rhsNodeClip] = rhsBoneNodeClipCache[i];

			unsigned int boneIndex = lhsBone->Index;

			if (boneIndex < startIndex || boneIndex > endIndex)
			{
				continue;
			}

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

	void NodeHierarchyInstance::calculateRootTransform(unsigned int startIndex, unsigned int endIndex)
	{
		const std::vector<fq::common::Bone>& bones = mNodeHierarchy->GetBones();

		mRootTransforms[0] = mLocalTransforms[0];

		for (size_t i = std::max<size_t>(startIndex, 1); i <= endIndex; ++i)
		{
			assert(bones[i].Index == i);
			mRootTransforms[i] = mLocalTransforms[i] * mRootTransforms[bones[i].ParentIndex];
		}
		for (size_t i = startIndex; i <= endIndex; ++i)
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