#pragma once

#include <filesystem>
#include "FQCommonLoader.h"

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
		struct MaterialTextureUseFlag
		{
			bool bUseBaseColor;
			bool bUseMetalness;
			bool bUseRoughness;
			bool bUseNormalness;
		};

		class IMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetTextureUseFlag(const MaterialTextureUseFlag& materialTextureUseFlag) abstract;
			virtual FQ_GRAPHICS void SetMaterialData(const fq::common::Material& materialData) abstract;
			virtual FQ_GRAPHICS void SetTextureBasePath(const std::filesystem::path& basePath) abstract;

			virtual FQ_GRAPHICS const MaterialTextureUseFlag& GetTextureUseFlag() const abstract;
			virtual FQ_GRAPHICS const fq::common::Material& GetMaterialData() const abstract;
			virtual FQ_GRAPHICS const std::filesystem::path& GetTextureBasePath() const abstract;

		protected:
			virtual ~IMaterial() = default;
		};
	}
#ifdef __cplusplus
}
#endif