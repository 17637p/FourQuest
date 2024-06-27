#pragma once
#include "Component.h"

#include <vector>
#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class Trail : public Component
	{
		using TrailInfo = graphics::TrailInfo;

	public:
		Trail();
		~Trail() = default;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		// interface
		void SetTrailObjectInterface(graphics::ITrailObject* trailObjectInterface) { mTrailObjectInterface = trailObjectInterface; }
		graphics::ITrailObject* GetTrailObjectInterface() const { return mTrailObjectInterface; }
		graphics::ParticleMaterialInfo GetParticleMaterialInfo() const { return mParticleMaterialInfo; }
		void SetParticleMaterialInfo(graphics::ParticleMaterialInfo data);

		// trail Info
		void SetTrailInfo(const TrailInfo& trailInfo) { mTrailInfo = trailInfo; TrySetTrailInfo(); }
		const TrailInfo& GetTrailInfo() const { return mTrailInfo; }
		void SetWidthRatios(const std::vector<DirectX::SimpleMath::Vector2>& widthRatios) { mTrailInfo.WidthRatios = widthRatios; TrySetTrailInfo(); }
		const std::vector<DirectX::SimpleMath::Vector2>& GetWidthRatios() const { return mTrailInfo.WidthRatios; }
		void SetColorRatios(const std::vector<DirectX::SimpleMath::Vector4>& colorRatios) { mTrailInfo.ColorRatios = colorRatios; TrySetTrailInfo(); }
		const std::vector<DirectX::SimpleMath::Vector4>& GetColorRatios() const { return mTrailInfo.ColorRatios; }
		void SetAlphaRatios(const std::vector<DirectX::SimpleMath::Vector2>& alphaRatios) { mTrailInfo.AlphaRatios = alphaRatios; TrySetTrailInfo(); }
		const std::vector<DirectX::SimpleMath::Vector2>& GetAlphaRatios() const { return mTrailInfo.AlphaRatios; }

		// material 
		std::shared_ptr<graphics::IParticleMaterial> GetParticleMaterial() const { return mParticleMaterial; }
		void SetParticleMaterial(std::shared_ptr<graphics::IParticleMaterial> material) { mParticleMaterial = material; }

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void TrySetTrailInfo() { if (mTrailObjectInterface) { mTrailObjectInterface->SetTrailInfo(mTrailInfo); } }

	private:
		graphics::ITrailObject* mTrailObjectInterface;
		std::shared_ptr<graphics::IParticleMaterial> mParticleMaterial;

		TrailInfo mTrailInfo;
		graphics::ParticleMaterialInfo mParticleMaterialInfo;
	};
}