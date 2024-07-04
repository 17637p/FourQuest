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
		class IStaticMesh
		{
		public:
			virtual FQ_GRAPHICS void* GetVertexBuffer() abstract;
			virtual FQ_GRAPHICS void* GetIndexBuffer() abstract;

			virtual FQ_GRAPHICS const fq::common::Mesh& GetMeshData() const abstract;

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
			virtual FQ_GRAPHICS void Create(const fq::common::AnimationClip& animationClip) abstract;
			// virtual FQ_GRAPHICS void SetName(const std::string& name) abstract;
			// virtual FQ_GRAPHICS const std::string& GetName() const abstract;

			virtual const fq::common::AnimationClip& GetAnimationClip() const abstract;

		protected:
			virtual ~IAnimation() = default;
		};

		class INodeHierarchyInstance;

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

		protected:
			virtual ~INodeHierarchy() = default;
		};

		class INodeHierarchyInstance
		{
		public:
			virtual FQ_GRAPHICS void SetBindPose() abstract;
			virtual FQ_GRAPHICS void Update(float timePos, std::shared_ptr<IAnimation> animation) abstract;
			virtual FQ_GRAPHICS void Update(float lhsTimePos, std::shared_ptr<IAnimation> lhsAnimation, float rhsTimePos, std::shared_ptr<IAnimation> rhsAnimation, float weight) abstract;

			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchy> GetNodeHierarchy() const abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetRootTransform(const std::string& boneName) const abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetRootTransform(size_t index) const abstract;
			virtual FQ_GRAPHICS const std::vector<DirectX::SimpleMath::Matrix>& GetRootTransforms() const abstract;

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