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
	}
#ifdef __cplusplus
}
#endif