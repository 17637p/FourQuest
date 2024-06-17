#pragma once

#include "FQCommonGraphics.h"

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
		// 그래픽스 종송적인 맴버를 갖지 않고 있는 클래스들
		class INode
		{
		public:

		private:
			unsigned int mIndex;
			unsigned int mParentNodeIndex;
			unsigned int mMeshIndex;
			std::string mName;
			std::shared_ptr<IMesh> mMesh;
			DirectX::SimpleMath::Matrix mParentMatrix;
			DirectX::SimpleMath::Matrix mRootMatrix;
			DirectX::SimpleMath::Matrix mOffsetMatrix; // toRootinvert;
		};

		class IModel
		{
		public:

		private:
			std::vector<std::shared_ptr<INode>> mNodes;
			std::vector<std::shared_ptr<IMesh>> mMeshes;
			std::vector<std::shared_ptr<IMaterial>> mMaterials;
			std::vector<std::shared_ptr<IAnimation>> mAnimations;
		};

		class IMesh
		{
		public:
			struct Subset
			{
				unsigned int VertexStart;
				unsigned int VertexCount;
				unsigned int IndexStart;
				unsigned int IndexCount;
				unsigned int MaterialIndex;
			};

			enum class EMeshType
			{
				Static,
				Skinned
			};

		public:

		private:
			unsigned int mNodeIndex;
			std::vector<Subset> mSubsets;
		};

		class IStaticMesh : public IMesh
		{
		public:
			struct Vertex
			{
				DirectX::SimpleMath::Vector3 Pos;
				DirectX::SimpleMath::Vector3 Normal;
				DirectX::SimpleMath::Vector3 Tangent;
				DirectX::SimpleMath::Vector2 Tex;
			};

		public:

		private:
			std::vector<Vertex> mVertices;
		};

		class ISkinnedMesh : public IMesh
		{
		public:
			struct Vertex
			{
				enum { MAX_BONE_COUNT = 4 };

				DirectX::SimpleMath::Vector3 Pos;
				DirectX::SimpleMath::Vector3 Normal;
				DirectX::SimpleMath::Vector3 Tangent;
				DirectX::SimpleMath::Vector2 Tex;
				int BoneIndices[MAX_BONE_COUNT];
				float BoneWeights[MAX_BONE_COUNT];
			};

		public:

		private:
			std::vector<Vertex> mVertices;
		};

		struct Keyframe
		{
			float TimePos = 0.f;
			DirectX::SimpleMath::Vector3 Translation = { 0.f, 0.f, 0.f };
			DirectX::SimpleMath::Vector3 Scale = { 1.f, 1.f, 1.f };
			DirectX::SimpleMath::Quaternion Rotation;
		};

		struct NodeKeyframe
		{
			std::string NodeName;

		};

		class IStaticAnimation
		{
		public:


		private:
			std::vector<Keyframe> mKeyframes;
		};

		class ISkinnedAnimation
		{

		};

		class IBoneHierarchy
		{

		};

		class IBoneHierarchyCache
		{

		};

		class IMaterial
		{
			friend class D3D11ModelManager;

		public:
			virtual FQ_GRAPHICS EMaterialType GetMaterialType() const abstract;

			virtual FQ_GRAPHICS void SetStandardMaterialInfo(const StandardMaterialInfo& materialInfo) { assert(false); throw; }
			virtual FQ_GRAPHICS const StandardMaterialInfo& GetStandardMaterialInfo() const { assert(false); throw; }
			virtual FQ_GRAPHICS StandardMaterialInfo& GetStandardMaterialInfo() { assert(false); throw; }

			virtual FQ_GRAPHICS void SetDecalMaterialInfo(const DecalMaterialInfo& materialInfo) { assert(false); throw; }
			virtual FQ_GRAPHICS const DecalMaterialInfo& GetDecalMaterialInfo() const { assert(false); throw; }
			virtual FQ_GRAPHICS DecalMaterialInfo& GetDecalMaterialInfo() { assert(false); throw; }

			virtual FQ_GRAPHICS void SetMaterialControllInfo(const MaterialControllInfo& materialControllInfo) abstract;
			virtual FQ_GRAPHICS const MaterialControllInfo& GetMaterialControllInfo() const abstract;

		protected:
			virtual ~IMaterial() = default;

		private:
			virtual void loadTexture(std::shared_ptr<class D3D11ResourceManager> resourceManager) abstract;
		};

		class IStandardMaterial
		{
		public:


		};

		class IDecalMaterial
		{
		public:


		};
	}
#ifdef __cplusplus
}
#endif