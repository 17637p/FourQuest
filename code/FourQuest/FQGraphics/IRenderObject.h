#pragma once

#include "../FQCommon/FQCommon.h"

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
			virtual FQ_GRAPHICS void UpdateTransform(const fq::common::Transform& transform) = 0;

		protected:
			~IStaticMeshObject() = default;
		};

		class ISkinnedMeshObject
		{
		public:
			virtual FQ_GRAPHICS void UpdateTransform(const fq::common::Transform& transform) = 0;
			virtual FQ_GRAPHICS void UpdateBoneMatrix(const std::vector<DirectX::SimpleMath::Matrix>& boneMatrices) = 0;

		protected:
			~ISkinnedMeshObject() = default;
		};
	}
#ifdef __cplusplus
}
#endif