#pragma once

#include <set>
#include <string>
#include <directxtk/SimpleMath.h>
#include "EObjectRenderType.h"
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
		class IMaterial;

		class IStaticMeshObject
		{
		public:
			// 지울 예정인 함수
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;

		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void SetObjectRenderType(EObjectRenderType renderType) = 0;
			virtual FQ_GRAPHICS void SetAlpha(float alpha) = 0;
			virtual FQ_GRAPHICS void SetUseShadow(bool bUseShadow) = 0;
			virtual FQ_GRAPHICS void SetAnimationTime(float timePos) = 0;
			virtual FQ_GRAPHICS bool SetAnimationKey(const std::string& animationKey) = 0;
			virtual FQ_GRAPHICS bool SetBlendAnimationKey(const std::array<std::string, 2> animKeys) = 0;
			virtual FQ_GRAPHICS bool SetBlendAnimationKey(const std::string& animKey, const std::string& blendAnimKey) = 0;
			virtual FQ_GRAPHICS void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) = 0;
			virtual FQ_GRAPHICS void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;
			virtual FQ_GRAPHICS EObjectRenderType GetObjectRenderType() const = 0;
			virtual FQ_GRAPHICS float GetAlpha() const = 0;
			virtual FQ_GRAPHICS bool GetUseShadow() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const = 0;
			virtual FQ_GRAPHICS float GetAnimationTime() const = 0;
			virtual FQ_GRAPHICS float GetBlendTime() const = 0;

			// Outline
			virtual FQ_GRAPHICS void SetOutlineColor(const DirectX::SimpleMath::Color& color) abstract;
			virtual FQ_GRAPHICS DirectX::SimpleMath::Color GetOutlineColor() const abstract;

			virtual FQ_GRAPHICS const std::vector<std::shared_ptr<IMaterial>>& GetMaterialInterfaces() const abstract;
			virtual FQ_GRAPHICS void SetMaterialInterfaces(const std::vector<std::shared_ptr<IMaterial>>& materials) abstract;

		protected:
			virtual ~IStaticMeshObject() = default;
		};

		class ISkinnedMeshObject
		{
		public:
			// 지울 예정인 함수
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void UpdateAnimationTime(float timePos) = 0;

		public:
			virtual FQ_GRAPHICS void SetBindPose() = 0;
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void SetAnimationTime(float timePos) = 0;
			virtual FQ_GRAPHICS void SetObjectRenderType(EObjectRenderType renderType) = 0;
			virtual FQ_GRAPHICS void SetAlpha(float alpha) = 0;
			virtual FQ_GRAPHICS void SetUseShadow(bool bUseShadow) = 0;
			virtual FQ_GRAPHICS bool SetAnimationKey(const std::string& animationKey) = 0;
			virtual FQ_GRAPHICS bool SetBlendAnimationKey(const std::array<std::string, 2> animKeys) = 0;
			virtual FQ_GRAPHICS bool SetBlendAnimationKey(const std::string& animKey, const std::string& blendAnimKey) = 0;
			virtual FQ_GRAPHICS void SetBlendAnimationTime(const std::array<float, 2>& timePos, float blendWeight) = 0;
			virtual FQ_GRAPHICS void SetBlendAnimationTime(float timePos, float blendTimePos, float blendWeight) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;
			virtual FQ_GRAPHICS float GetAnimationTime() const = 0;
			virtual FQ_GRAPHICS float GetBlendTime() const = 0;
			virtual FQ_GRAPHICS EObjectRenderType GetObjectRenderType() const = 0;
			virtual FQ_GRAPHICS float GetAlpha() const = 0;
			virtual FQ_GRAPHICS bool GetUseShadow() const = 0;
			virtual FQ_GRAPHICS std::set<std::string> GetAnimationKeys() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const = 0;

			// Outline
			virtual FQ_GRAPHICS void SetOutlineColor(const DirectX::SimpleMath::Color& color) abstract;
			virtual FQ_GRAPHICS DirectX::SimpleMath::Color GetOutlineColor() const abstract;

			virtual FQ_GRAPHICS const std::vector<std::shared_ptr<IMaterial>>& GetMaterialInterfaces() const abstract;
			virtual FQ_GRAPHICS void SetMaterialInterfaces(const std::vector<std::shared_ptr<IMaterial>>& materialInterfaces) abstract;

			// to do : 본 계층 구조 노출
			// to do : 특정 본의 toRoot 노출

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
			virtual FQ_GRAPHICS void SetInfo(const ParticleInfo& info) abstract;
			virtual FQ_GRAPHICS void SetFrameTime(float frameTime) abstract;
			virtual FQ_GRAPHICS void SetIsReset(bool bIsReset) abstract;
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;
			virtual FQ_GRAPHICS void SetIsEmit(bool bIsEmit) abstract;
			virtual FQ_GRAPHICS void SetIsRenderDebug(bool bIsRenderDebug) abstract;

			virtual FQ_GRAPHICS const ParticleInfo& GetInfo() const abstract;
			virtual FQ_GRAPHICS bool GetIsReset() const abstract;
			virtual FQ_GRAPHICS float GetFrameTIme() const abstract;
			virtual FQ_GRAPHICS DirectX::SimpleMath::Matrix GetTransform() const abstract;
			virtual FQ_GRAPHICS bool GetIsEmit() const abstract;
			virtual FQ_GRAPHICS bool GetIsRenderDebug() const abstract;

		protected:
			virtual ~IParticleObject() = default;
		};
	}
#ifdef __cplusplus
}
#endif