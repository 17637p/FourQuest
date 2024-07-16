#pragma once

#include <set>
#include <string>
#include <directxtk/SimpleMath.h>
#include "FQCommonGraphics.h"
#include "FQCommonLoader.h"
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
		class IMaterial;
		class IDecalMaterial;
		class IParticleMaterial;

		class IStaticMeshObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Info
			virtual FQ_GRAPHICS void SetMeshObjectInfo(const MeshObjectInfo& info) abstract;
			virtual FQ_GRAPHICS const MeshObjectInfo& GetMeshObjectInfo() const abstract;

			// Material
			virtual FQ_GRAPHICS void SetMaterials(const std::vector<std::shared_ptr<IMaterial>>& materials) abstract;
			virtual FQ_GRAPHICS const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const abstract;

			// Mesh
			virtual FQ_GRAPHICS void SetStaticMesh(std::shared_ptr<IStaticMesh> staticMesh) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> GetStaticMesh() const abstract;

			// NodeHierarchy
			virtual FQ_GRAPHICS void SetNodeHierarchyInstance(std::shared_ptr<INodeHierarchyInstance> nodeHierarchyInstance) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchyInstance> GetNodeHierarchyInstance() const abstract;
			virtual FQ_GRAPHICS void SetReferenceBoneIndex(size_t index) abstract;
			virtual FQ_GRAPHICS size_t GetReferenceBoneIndex() const abstract;

			// UVAnimInstance
			virtual FQ_GRAPHICS void SetUVAnimationInstance(std::shared_ptr<IUVAnimationInstance> uvAnimationInstance) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimationInstance> GetUVAnimationInstanceOrNull() const abstract;

		protected:
			virtual ~IStaticMeshObject() = default;
		};

		class ISkinnedMeshObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Info
			virtual FQ_GRAPHICS void SetMeshObjectInfo(const MeshObjectInfo& info) abstract;
			virtual FQ_GRAPHICS const MeshObjectInfo& GetMeshObjectInfo() const abstract;

			// Material
			virtual FQ_GRAPHICS void SetMaterials(const std::vector<std::shared_ptr<IMaterial>>& materials) abstract;
			virtual FQ_GRAPHICS const std::vector<std::shared_ptr<IMaterial>>& GetMaterials() const abstract;

			// Mesh
			virtual FQ_GRAPHICS void SetSkinnedMesh(std::shared_ptr<ISkinnedMesh> skinnedMesh) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<ISkinnedMesh> GetSkinnedMesh() const abstract;

			// NodeHierarchy
			virtual FQ_GRAPHICS void SetNodeHierarchyInstance(std::shared_ptr<INodeHierarchyInstance> nodeHierarchyInstance) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<INodeHierarchyInstance> GetNodeHierarchyInstance() const abstract;

		protected:
			virtual ~ISkinnedMeshObject() = default;
		};

		class ITerrainMeshObject
		{
		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;

			virtual FQ_GRAPHICS DirectX::SimpleMath::Matrix GetTransform() const abstract;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const abstract;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const abstract;

			virtual FQ_GRAPHICS const fq::common::Mesh& GetMeshData() const abstract;
			virtual FQ_GRAPHICS int* GetHeightData() const abstract;

		protected:
			virtual ~ITerrainMeshObject() = default;
		};

		class IImageObject
		{
		public:
			virtual ~IImageObject() = default;

			virtual FQ_GRAPHICS void SetStartX(float startX) abstract;
			virtual FQ_GRAPHICS void SetStartY(float startY) abstract;
			virtual FQ_GRAPHICS float GetStartX() abstract;
			virtual FQ_GRAPHICS float GetStartY() abstract;

			virtual FQ_GRAPHICS void SetWidth(float width) abstract;
			virtual FQ_GRAPHICS void SetHeight(float height) abstract;
			virtual FQ_GRAPHICS float GetWidth() abstract;
			virtual FQ_GRAPHICS float GetHeight() abstract;

			virtual FQ_GRAPHICS void SetLayer(unsigned int layer) abstract;
			virtual FQ_GRAPHICS unsigned int GetLayer() abstract;

			virtual FQ_GRAPHICS void SetXRatio(float xRatio) abstract;
			virtual FQ_GRAPHICS float GetXRatio() abstract;
			virtual FQ_GRAPHICS void SetYRatio(float yRatio) abstract;
			virtual FQ_GRAPHICS float GetYRatio() abstract;

			virtual FQ_GRAPHICS void SetAlpha(float alpha) abstract;
			virtual FQ_GRAPHICS float GetAlpha() abstract;

			virtual FQ_GRAPHICS void SetRotation(float angle) abstract;
			virtual FQ_GRAPHICS float GetRotation() const abstract;

			virtual FQ_GRAPHICS void SetScaleX(float scaleX) abstract;
			virtual FQ_GRAPHICS void SetScaleY(float scaleY) abstract;
			virtual FQ_GRAPHICS float GetScaleX() const abstract;
			virtual FQ_GRAPHICS float GetScaleY() const abstract;

			virtual FQ_GRAPHICS void SetImagePath(const std::string& texturePath) abstract;
			virtual FQ_GRAPHICS std::string GetImagePath() abstract;
		};

		class IParticleObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Info
			virtual FQ_GRAPHICS void SetInfo(const ParticleInfo& info) abstract;
			virtual FQ_GRAPHICS const ParticleInfo& GetInfo() const abstract;
			virtual FQ_GRAPHICS ParticleInfo& GetInfo() abstract;

			// Material
			virtual FQ_GRAPHICS void SetIParticleMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> GetIParticleMaterial() const abstract;

			// FrameTime
			virtual FQ_GRAPHICS void SetFrameTime(float frameTime) abstract;
			virtual FQ_GRAPHICS float GetFrameTIme() const abstract;

		protected:
			virtual ~IParticleObject() = default;
		};

		class IDecalObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Info
			virtual FQ_GRAPHICS void SetDecalInfo(const DecalInfo& decalInfo) abstract;
			virtual FQ_GRAPHICS const DecalInfo& GetDecalInfo() const abstract;

			// Material
			virtual FQ_GRAPHICS void SetIDecalMaterial(std::shared_ptr<IDecalMaterial> iDecalMaterial) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IDecalMaterial> GetDecalMaterial() const abstract;

			// UVAnimInstance
			virtual FQ_GRAPHICS void SetUVAnimationInstance(std::shared_ptr<IUVAnimationInstance> uvAnimationInstance) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IUVAnimationInstance> GetUVAnimationInstanceOrNull() const abstract;

		protected:
			virtual ~IDecalObject() = default;
		};

		class ITrailObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Info
			virtual FQ_GRAPHICS void SetTrailInfo(const TrailInfo& trailInfo) abstract;
			virtual FQ_GRAPHICS const TrailInfo& GetTrailInfo() const abstract;
			virtual FQ_GRAPHICS TrailInfo& GetTrailInfo() abstract;

			// Material
			virtual FQ_GRAPHICS void SetIParticleMaterial(std::shared_ptr<IParticleMaterial> iParticleMaterial) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IParticleMaterial> GetIParticleMaterial() const abstract;

			// FrameTime
			virtual FQ_GRAPHICS void SetFrameTime(float frameTime) abstract;
			virtual FQ_GRAPHICS float GetFrameTIme() const abstract;

		protected:
			virtual ~ITrailObject() = default;
		};

		class IProbeObject
		{
		public:
			// Transform
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;

			// Mesh
			virtual FQ_GRAPHICS void SetStaticMesh(std::shared_ptr<IStaticMesh> staticMesh) abstract;
			virtual FQ_GRAPHICS std::shared_ptr<IStaticMesh> GetStaticMesh() const abstract;

			// LightProbe
			virtual FQ_GRAPHICS void SetIndex(int index) abstract;
			virtual FQ_GRAPHICS int GetIndex() abstract;
		protected:
			virtual ~IProbeObject() = default;
		};
#ifdef __cplusplus
	}
#endif
}