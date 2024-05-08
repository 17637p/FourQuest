#pragma once

#include <set>
#include <string>
#include <directxtk/SimpleMath.h>

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
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;

		protected:
			virtual ~IStaticMeshObject() = default;
		};

		class ISkinnedMeshObject
		{
		public:
			virtual FQ_GRAPHICS void UpdateTransform(const DirectX::SimpleMath::Matrix& transform) = 0;
			virtual FQ_GRAPHICS void UpdateAnimationTime(float timePos) = 0;

			virtual FQ_GRAPHICS bool SetAnimationKey(const std::string& animationKey) = 0;

			virtual FQ_GRAPHICS const DirectX::SimpleMath::Matrix& GetTransform() const = 0;
			virtual FQ_GRAPHICS std::set<std::string> GetAnimationKeys() const = 0;
			// to do : 본 계층 구조 노출
			// to do : 특정 본의 toRoot 노출

		protected:
			virtual ~ISkinnedMeshObject() = default;
		};
	}
#ifdef __cplusplus
}
#endif