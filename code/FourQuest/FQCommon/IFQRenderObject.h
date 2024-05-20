#pragma once

#include <set>
#include <string>
#include <directxtk/SimpleMath.h>
#include "EObjectRenderType.h"

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
		class IStaticMeshObject
		{
		public:
			// ���� ������ �Լ�
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;

		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void SetObjectRenderType(EObjectRenderType renderType) = 0;
			virtual FQ_GRAPHICS void SetAlpha(float alpha) = 0;
			virtual FQ_GRAPHICS void SetUseShadow(bool bUseShadow) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;
			virtual FQ_GRAPHICS EObjectRenderType GetObjectRenderType() const = 0;
			virtual FQ_GRAPHICS float GetAlpha() const = 0;
			virtual FQ_GRAPHICS bool GetUseShadow() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const = 0;

		protected:
			virtual ~IStaticMeshObject() = default;
		};

		class ISkinnedMeshObject
		{
		public:
			// ���� ������ �Լ�
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void UpdateAnimationTime(float timePos) = 0;

		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void SetAnimationTime(float timePos) = 0;
			virtual FQ_GRAPHICS void SetObjectRenderType(EObjectRenderType renderType) = 0;
			virtual FQ_GRAPHICS void SetAlpha(float alpha) = 0;
			virtual FQ_GRAPHICS void SetUseShadow(bool bUseShadow) = 0;
			virtual FQ_GRAPHICS bool SetAnimationKey(const std::string& animationKey) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;
			virtual FQ_GRAPHICS float GetAnimationTime() const = 0;
			virtual FQ_GRAPHICS EObjectRenderType GetObjectRenderType() const = 0;
			virtual FQ_GRAPHICS float GetAlpha() const = 0;
			virtual FQ_GRAPHICS bool GetUseShadow() const = 0;
			virtual FQ_GRAPHICS std::set<std::string> GetAnimationKeys() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const = 0;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const = 0;
			 
			// to do : �� ���� ���� ����
			// to do : Ư�� ���� toRoot ����

		protected:
			virtual ~ISkinnedMeshObject() = default;
		};

		// Ÿ�� ��� ������ �߰��ؾ߰���
		class ITerrainMeshObject
		{
		public:
			virtual FQ_GRAPHICS void SetTransform(const DirectX::SimpleMath::Matrix& transform) abstract;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const abstract;
			virtual FQ_GRAPHICS DirectX::BoundingBox GetRenderBoundingBox() const abstract;
			virtual FQ_GRAPHICS DirectX::BoundingSphere GetRenderBoundingSphere() const abstract;

		protected:
			virtual ~ITerrainMeshObject() = default;
		};
	}
#ifdef __cplusplus
}
#endif