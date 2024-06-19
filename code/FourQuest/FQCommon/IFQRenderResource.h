#pragma once

#include <filesystem>
#include "FQCommonLoader.h"
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
		class IMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const MaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const MaterialInfo& GetInfo() const abstract;

		protected:
			virtual ~IMaterial() = default;
		};

		class IParticleMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const ParticleMaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const ParticleMaterialInfo& GetInfo() const abstract;

		protected:
			virtual ~IParticleMaterial() = default;
		};

		class IDecalMaterial
		{
		public:
			virtual FQ_GRAPHICS void SetInfo(const DecalMaterialInfo& info) abstract;
			virtual FQ_GRAPHICS const DecalMaterialInfo& GetInfo() const abstract;

		protected:
			virtual ~IDecalMaterial() = default;
		};
	}
#ifdef __cplusplus
}
#endif