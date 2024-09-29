#pragma once

#include <filesystem>
#include <string>
#include <set>
#include <map>
#include "FQCommonLoader.h"
#include "FQCommonGraphics.h"
#include "IFQRenderResource.h"

#ifdef FQ_GRAPHICS_EXPORT
#define FQ_GRAPHICS __declspec(dllexport)
#else
#define FQ_GRAPHICS __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	namespace fq::graphics
	{
		enum class EStaticMeshType
		{
			Default,
			Static,
			VertexColor,
		};

		class IStaticMesh
		{
		public:
			virtual FQ_GRAPHICS void* GetVertexBuffer() abstract;
			virtual FQ_GRAPHICS void* GetIndexBuffer() abstract;

			virtual FQ_GRAPHICS const fq::common::Mesh& GetMeshData() const abstract;
			virtual FQ_GRAPHICS EStaticMeshType GetStaticMeshType() const abstract;

		protected:
			virtual ~IStaticMesh() = default;
		};

		class ISkinnedMesh
		{
		public:
			virtual FQ_GRAPHICS void* GetVertexBuffer() abstract;
			virtual FQ_GRAPHICS void* GetIndexBuffer() abstract;

			virtual FQ_GRAPHICS const fq::common::Mesh& GetMeshData() const abstract;

		protected:
			virtual ~ISkinnedMesh() = default;
		};

		class IAnimation
		{
		public:
			// virtual FQ_GRAPHICS void Create(const fq::common::AnimationClip& animationClip) abstract;
			// virtual FQ_GRAPHICS void SetName(const std::string& name) abstract;
			// virtual FQ_GRAPHICS const std::string& GetName() const abstract;

			virtual FQ_GRAPHICS const fq::common::AnimationClip& GetAnimationClip() const abstract;

		protected:
			virtual ~IAnimation() = default;
		};

		class IUVAnimationInstance
		{
		public:
			virtual FQ_GRAPHICS void SetTimePos(float timePos) abstract;
			virtual FQ_GRAPHICS float GetTimePos() const abstract;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTexTransform(const std::string& nodeName) const abstract;

		protected:
			virtual ~IUVAnimationInstance() = default;
		};

		class IUVAnimation
		{
		public:
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimationInstance> CreateUVAnimationInstance() abstract;
			virtual FQ_GRAPHICS const fq::common::UVAnimationClip& GetUVAnimationClip() const abstract;
			virtual FQ_GRAPHICS void SetUVAnimationClip(const fq::common::UVAnimationClip& clip) abstract;

		protected:
			virtual ~IUVAnimation() = default;
		};


		class INodeHierarchyInstance;

		/// <summary>
		/// �� �����Ϳ� �����Ǵ� ��� ���� ������ �����ϴ� Ŭ����
		/// </summary>
		class INodeHierarchy
		{
		public:
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchyInstance> CreateNodeHierarchyInstance() abstract;

			virtual FQ_GRAPHICS void RegisterAnimation(std::shared_ptr<IAnimation> animation) abstract;
			virtual FQ_GRAPHICS void UnregisterAnimation(std::shared_ptr<IAnimation> animation) abstract;
			virtual FQ_GRAPHICS void UnregisterAllAnimations() abstract;
			virtual FQ_GRAPHICS const std::set<std::shared_ptr<IAnimation>>& GetRegisterAnimations() const abstract;

			virtual FQ_GRAPHICS const std::vector<fq::common::Bone>& GetBones() const abstract;
			virtual FQ_GRAPHICS unsigned int GetBoneIndex(const std::string& boneName) const abstract;
			virtual FQ_GRAPHICS bool TryGetBoneIndex(const std::string& boneName, unsigned int* outBoneIndex) const abstract;

			virtual FQ_GRAPHICS void SetSpineBoneName(const std::string& boneName) abstract;
			virtual FQ_GRAPHICS void SetSpineBoneIndex(unsigned int boneIndex) abstract;
			virtual FQ_GRAPHICS unsigned int GetSpineIndex() const abstract;
			virtual FQ_GRAPHICS unsigned int GetUpperBodyEndIndex() const abstract;
			virtual FQ_GRAPHICS unsigned int GetLowerBodyStartIndex() const abstract;
			virtual FQ_GRAPHICS unsigned int GetEndIndex() const abstract;

		protected:
			virtual ~INodeHierarchy() = default;
		};

		/// <summary>
		/// �޽� ������Ʈ�� ������ �ִϸ��̼� ����(��ȯ ��� �ȷ�Ʈ)�� �����ϴ� Ŭ����
		/// </summary>
		class INodeHierarchyInstance
		{
		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			virtual FQ_GRAPHICS void SetBindPose() abstract;
			virtual FQ_GRAPHICS void SetBindPoseLocalTransform() abstract;
			virtual FQ_GRAPHICS void Update(float timePos, const std::shared_ptr<IAnimation>& animation) abstract;
			virtual FQ_GRAPHICS void Update(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) abstract;
			virtual FQ_GRAPHICS void UpdateGPUData(float timePos, const std::shared_ptr<IAnimation>& animation) abstract;
			virtual FQ_GRAPHICS void UpdateGPUData(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) abstract;
			virtual FQ_GRAPHICS void UpdateByLocalTransform() abstract;
			virtual FQ_GRAPHICS void UpdateByLocalTransform(unsigned int startIndex, unsigned int endIndex) abstract;
			virtual FQ_GRAPHICS void UpdateByLocalTransform(float timePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight) abstract;
			virtual FQ_GRAPHICS void UpdateLocalTransformRange(float timePos, const std::shared_ptr<IAnimation>& animation, unsigned int startIndex, unsigned int endIndex) abstract;
			virtual FQ_GRAPHICS void UpdateLocalTransformRange(float lhsTimePos, const std::shared_ptr<IAnimation>& lhsAnimation, float rhsTimePos, const std::shared_ptr<IAnimation>& rhsAnimation, float weight, unsigned int startIndex, unsigned int endIndex) abstract;

			virtual FQ_GRAPHICS void SetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS bool TrySetLocalTransform(size_t index, const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> GetNodeHierarchy() const abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const abstract;
			virtual FQ_GRAPHICS const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransposedFinalTransform(size_t index) const abstract;

		protected:
			virtual ~INodeHierarchyInstance() = default;
		};
		class IMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const MaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const MaterialInfo& GetInfo() const abstract;

			virtual FQ_GRAPHICS const std::string& GetName() const abstract;

		protected:
			virtual ~IMaterial() = default;
		};

		class IParticleMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const ParticleMaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const ParticleMaterialInfo& GetInfo() const abstract;

			virtual FQ_GRAPHICS const std::string& GetName() const abstract;

		protected:
			virtual ~IParticleMaterial() = default;
		};

		class IDecalMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const DecalMaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const DecalMaterialInfo& GetInfo() const abstract;

			virtual FQ_GRAPHICS const std::string& GetName() const abstract;

		protected:
			virtual ~IDecalMaterial() = default;
		};
	}
#ifdef __cplusplus
}
#endif