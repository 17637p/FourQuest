#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <directxtk/SimpleMath.h>
#include "../FQCommon/FQCommonLoader.h"
#include "../FQCommon/IFQRenderResource.h"
#include "AnimationHelper.h"

namespace fq::graphics
{
	class NodeHierarchy : public INodeHierarchy, public std::enable_shared_from_this<NodeHierarchy>
	{
		using BoneNodeClipCache = std::pair<const struct fq::common::Bone*, const struct fq::common::NodeClip*>;

	public:
		NodeHierarchy() = default;
		NodeHierarchy(const fq::common::Model& model);
		NodeHierarchy(const std::vector<fq::common::Node> nodes);
		~NodeHierarchy() = default;

		void Create(const fq::common::Model& model);
		void Create(const std::vector<fq::common::Node> nodes);

		virtual std::shared_ptr<INodeHierarchyInstance> CreateNodeHierarchyInstance() override;

		virtual void RegisterAnimation(std::shared_ptr<IAnimation> animation) override;
		virtual void UnregisterAnimation(std::shared_ptr<IAnimation> animation) override { mAnimationCaches.erase(animation); mRegisterAnimations.clear(); }
		virtual void UnregisterAllAnimations() override { mAnimationCaches.clear(); mRegisterAnimations.clear(); }
		virtual const std::set<std::shared_ptr<IAnimation>>& GetRegisterAnimations() const override { return mRegisterAnimations; }

		virtual const std::vector<fq::common::Bone>& GetBones() const override { return mBones; }
		virtual unsigned int GetBoneIndex(const std::string& boneName) const override;
		virtual bool TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex) const override;
		const std::map<std::shared_ptr<IAnimation>, std::vector<BoneNodeClipCache>>& GetAnimationCaches() const { return mAnimationCaches; }

	private:
		std::vector<fq::common::Bone> mBones;
		std::map<std::shared_ptr<IAnimation>, std::vector<BoneNodeClipCache>> mAnimationCaches;
		std::set<std::shared_ptr<IAnimation>> mRegisterAnimations;
	};

	class NodeHierarchyInstance : public INodeHierarchyInstance
	{
	public:
		NodeHierarchyInstance(std::shared_ptr<INodeHierarchy> boneHierarchy);
		~NodeHierarchyInstance() = default;

		virtual void SetBindPose() override;
		virtual void Update(float timePos, const std::shared_ptr<IAnimation>& animation) override;
		virtual void Update(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) override; // 블렌딩 처리는 애니메이션이 cache로 등록된 경우만 사용 가능

		virtual std::shared_ptr<INodeHierarchy> GetNodeHierarchy() const override { return mNodeHierarchy; }
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const override { size_t index = mNodeHierarchy->GetBoneIndex(boneName); return mRootTransforms[index]; }
		virtual const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const override { return mRootTransforms[index]; }
		virtual const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const override { return mRootTransforms; }

		const std::vector<DirectX::SimpleMath::Matrix> GetTransposedFinalTransforms() const { return mTransposedFinalTransforms; }

	private:
		void clear();
		void calculateRootTransform();

	private:
		std::shared_ptr<INodeHierarchy> mNodeHierarchy;
		std::vector<DirectX::SimpleMath::Matrix> mRootTransforms;
		std::vector<DirectX::SimpleMath::Matrix> mTransposedFinalTransforms;
	};
}
