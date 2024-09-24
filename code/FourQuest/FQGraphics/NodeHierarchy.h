#pragma once

#include <array>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <directxtk/SimpleMath.h>

#include "AnimationHelper.h"
#include "ConstantBufferStructure.h"
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::graphics
{
	class D3D11TextureArray;
}

namespace fq::graphics
{
	struct AnimationKeyFrames
	{
		enum { MAX_MODEL_TRANSFORMS = 128 };
		enum { MAX_MODEL_KEYFRAMES = 500 };

		using KeyFrameArrayType = std::array<DirectX::SimpleMath::Matrix, MAX_MODEL_TRANSFORMS>;
		std::array<KeyFrameArrayType, MAX_MODEL_KEYFRAMES> KeyFrames;
	};

	class NodeHierarchy : public INodeHierarchy, public std::enable_shared_from_this<NodeHierarchy>
	{
		using BoneNodeClipCache = std::pair<const struct fq::common::Bone*, const struct fq::common::NodeClip*>;

	public:
		NodeHierarchy() = default;
		NodeHierarchy(const fq::common::Model& model);
		NodeHierarchy(const std::vector<fq::common::Node>& nodes);
		~NodeHierarchy() = default;

		void Create(const std::vector<fq::common::Node>& nodes);

		virtual std::shared_ptr<INodeHierarchyInstance> CreateNodeHierarchyInstance() override;

		virtual void RegisterAnimation(std::shared_ptr<IAnimation> animation) override;
		virtual void UnregisterAnimation(std::shared_ptr<IAnimation> animation) override { mAnimationCaches.erase(animation); mRegisterAnimations.clear(); }
		virtual void UnregisterAllAnimations() override { mAnimationCaches.clear(); mRegisterAnimations.clear(); }
		virtual const std::set<std::shared_ptr<IAnimation>>& GetRegisterAnimations() const override { return mRegisterAnimations; }

		virtual const std::vector<fq::common::Bone>& GetBones() const override { return mBones; }
		virtual unsigned int GetBoneIndex(const std::string& boneName) const override;
		virtual bool TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex) const override;

		virtual void SetSpineBoneName(const std::string& boneName) override;
		virtual void SetSpineBoneIndex(unsigned int boneIndex) override;
		virtual unsigned int GetSpineIndex() const override { return mSpineIndex; }
		virtual unsigned int GetUpperBodyEndIndex() const override { return mUpperBodyEndIndex; }
		virtual unsigned int GetLowerBodyStartIndex() const override { return mLowerBodyStartIndex; }
		virtual unsigned int GetEndIndex() const override { return mBones.size() - 1; }

		const std::map<std::shared_ptr<IAnimation>, std::vector<BoneNodeClipCache>>& GetAnimationCaches() const { return mAnimationCaches; }

		// 인스턴싱 관련 함수
		bool GetIsCreatedAnimationTexture() { return mAnimationKeyframeTexture != nullptr && !mAnimationKeyframes.empty() && !mRegisterAnimationIndexMap.empty(); }
		void CreateAnimationTexture(const std::shared_ptr<class D3D11Device>& device);
		void ReleaseAnimationTexture();
		const std::map<std::shared_ptr<IAnimation>, size_t>& GetRegisterAnimationIndexMap() const { return mRegisterAnimationIndexMap; }
		const std::vector<AnimationKeyFrames>& GetAnimationKeyframes() const { return mAnimationKeyframes; }
		const std::shared_ptr<D3D11TextureArray>& GetAnimationKeyframeTexture() const { return mAnimationKeyframeTexture; }

	private:
		std::vector<fq::common::Bone> mBones;
		std::map<std::shared_ptr<IAnimation>, std::vector<BoneNodeClipCache>> mAnimationCaches;
		std::set<std::shared_ptr<IAnimation>> mRegisterAnimations;

		// 인스턴싱 관련 데이터
		std::map<std::shared_ptr<IAnimation>, size_t> mRegisterAnimationIndexMap;
		std::vector<AnimationKeyFrames> mAnimationKeyframes;
		std::shared_ptr<D3D11TextureArray> mAnimationKeyframeTexture;

		unsigned int mSpineIndex;
		unsigned int mUpperBodyEndIndex;
		unsigned int mLowerBodyStartIndex;
	};

	class NodeHierarchyInstance : public INodeHierarchyInstance
	{
	public:
		NodeHierarchyInstance(std::shared_ptr<INodeHierarchy> boneHierarchy);
		~NodeHierarchyInstance() = default;

		virtual void SetTransform(const DirectX::SimpleMath::Matrix& transform) override { mTransform = transform; }
		virtual const DirectX::SimpleMath::Matrix& GetTransform() const override { return mTransform; }

		virtual void SetBindPose() override;
		virtual void SetBindPoseLocalTransform() override;
		virtual void Update(float timePos, const std::shared_ptr<IAnimation>& animation) override;
		virtual void Update(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) override; // 블렌딩 처리는 애니메이션이 cache로 등록된 경우만 사용 가능
		virtual void UpdateGPUData(float timePos, const std::shared_ptr<IAnimation>& animation) override;
		virtual void UpdateGPUData(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) override; // 블렌딩 처리는 애니메이션이 cache로 등록된 경우만 사용 가능
		virtual void UpdateByLocalTransform() override;
		virtual void UpdateByLocalTransform(float timePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) override;
		virtual void UpdateLocalTransformRange(float timePos, const std::shared_ptr<IAnimation>& animation, unsigned int startIndex, unsigned int endIndex) override;
		virtual void UpdateLocalTransformRange(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight, unsigned int startIndex, unsigned int endIndex) override;

		virtual void SetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform);
		virtual bool TrySetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform);

		virtual std::shared_ptr<INodeHierarchy> GetNodeHierarchy() const override { return mNodeHierarchy; }
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const override { size_t index = mNodeHierarchy->GetBoneIndex(boneName); return mRootTransforms[index]; }
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const override { return mRootTransforms[index]; }
		virtual const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const override { return mRootTransforms; }
		virtual const DirectX::SimpleMath::Matrix& GetTransposedFinalTransform(size_t index) const override { return mTransposedFinalTransforms[index]; }
		const std::vector<DirectX::SimpleMath::Matrix> GetTransposedFinalTransforms() const { return mTransposedFinalTransforms; }

		// 인스턴싱 관련 함수
		const TweenDesc& GeTweenDesc() const { return mTweenDesc; }

	private:
		void clear();
		void calculateRootTransform();
		void setBindPoseLocal();

	private:
		DirectX::SimpleMath::Matrix mTransform;
		std::shared_ptr<INodeHierarchy> mNodeHierarchy;
		std::vector<DirectX::SimpleMath::Matrix> mLocalTransforms;
		std::vector<DirectX::SimpleMath::Matrix> mRootTransforms;
		std::vector<DirectX::SimpleMath::Matrix> mTransposedFinalTransforms;

		TweenDesc mTweenDesc;
	};
}
