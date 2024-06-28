#pragma once
#include "Component.h"

#include <vector>
#include "../FQCommon/IFQRenderObject.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class Particle : public Component
	{
		using ParticleInfo = graphics::ParticleInfo;

	public:
		Particle();
		~Particle();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		fq::graphics::IParticleObject* GetParticleObject() const { return mParticleObject; }
		void SetParticleObject(fq::graphics::IParticleObject* val) { mParticleObject = val; }

		fq::graphics::ParticleInfo GetParticleInfomation() const { return mParticleInfo; }
		void SetParticleInfomation(fq::graphics::ParticleInfo val);

		ParticleInfo::Main GetMainParticleInfo()const { return mParticleInfo.MainData; }
		void SetMainParticleInfo(ParticleInfo::Main mainInfo);

		ParticleInfo::Emission GetEmissionInfo()const { return mParticleInfo.EmissionData; }
		void SetEmissionInfo(ParticleInfo::Emission data);

		ParticleInfo::Shape GetShapeInfo()const { return mParticleInfo.ShapeData; }
		void SetShapeInfo(ParticleInfo::Shape data);

		ParticleInfo::VelocityOverLifetime GetVelocityOverLifetimeData()const { return mParticleInfo.VelocityOverLifetimeData; }
		void SetVelocityOverLifetimeData(ParticleInfo::VelocityOverLifetime data);

		ParticleInfo::LimitVelocityOverLifetime GetLimitVelocityOverLifeTimeData()const { return mParticleInfo.LimitVelocityOverLifetimeData; }
		void SetLimitVelocityOverLifeTimeData(ParticleInfo::LimitVelocityOverLifetime data);

		ParticleInfo::ForceOverLifetime GetForceOverLifetimeData()const { return mParticleInfo.ForceOverLifeTimeData; }
		void SetForceOverLifetimeData(ParticleInfo::ForceOverLifetime data);

		const std::vector<DirectX::SimpleMath::Vector4>& GetColorRatios() const { return mParticleInfo.ColorOverLifetimeData.ColorRatios; }
		void SetColorRatios(const std::vector<DirectX::SimpleMath::Vector4>& colorRatios) { mParticleInfo.ColorOverLifetimeData.ColorRatios = colorRatios; tryUpdateObjectInfo(); }
		const std::vector<DirectX::SimpleMath::Vector2>& GetAlphaRatios() const { return mParticleInfo.ColorOverLifetimeData.AlphaRatios; }
		void SetAlphaRatios(const std::vector<DirectX::SimpleMath::Vector2>& alphaRatios) { mParticleInfo.ColorOverLifetimeData.AlphaRatios = alphaRatios; tryUpdateObjectInfo(); }
		ParticleInfo::ColorOverLifetime GetColorOverLifeTime()const { return mParticleInfo.ColorOverLifetimeData; }
		void SetColorOverLifeTime(ParticleInfo::ColorOverLifetime data);

		ParticleInfo::SizeOverLifetime GetSizeOverLifetime()const { return mParticleInfo.SizeOverLifetimeData; }
		void SetSizeOverLifetime(ParticleInfo::SizeOverLifetime data);

		ParticleInfo::RotationOverLifetime GetRotationOverLifetimeData()const { return mParticleInfo.RotationOverLifetimeData; }
		void SetRotationOverLifetimeData(ParticleInfo::RotationOverLifetime data);

		ParticleInfo::Render GetRenderData()const { return mParticleInfo.RenderData; }
		void SetRenderData(ParticleInfo::Render data);

		ParticleInfo::Instance GetInstanceData() const { return mParticleInfo.InstanceData; }
		void SetInstanceData(ParticleInfo::Instance data);

		std::vector<ParticleInfo::Emission::Burst> GetEmissionBursts()const { return mParticleInfo.EmissionData.Bursts; }
		void SetEmissionBursts(std::vector<ParticleInfo::Emission::Burst> bursts);

		float GetParticlesPerSecond()const { return mParticleInfo.EmissionData.ParticlesPerSecond; }
		void SetParticlesPerSecond(float data);

		std::shared_ptr<graphics::IParticleMaterial> GetParticleMaterial() const { return mParticleMaterial; }
		void SetParticleMaterial(std::shared_ptr<graphics::IParticleMaterial> material) { mParticleMaterial = material; }

		graphics::ParticleMaterialInfo GetParticleMaterialInfo() const { return mParticleMaterialInfo; }
		void SetParticleMaterialInfo(graphics::ParticleMaterialInfo data);

	private:
		entt::meta_handle GetHandle() override { return *this; }

		void tryUpdateObjectInfo()
		{
			if (mParticleObject != nullptr)
			{
				mParticleObject->SetInfo(mParticleInfo);
			}
		}
		void OnStart() override;
		void OnUpdate(float dt) override;

	private:
		graphics::IParticleObject* mParticleObject;
		std::shared_ptr<graphics::IParticleMaterial> mParticleMaterial;
		graphics::ParticleInfo mParticleInfo;
		graphics::ParticleMaterialInfo mParticleMaterialInfo;
	};

}