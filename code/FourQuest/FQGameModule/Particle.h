#pragma once
#include "Component.h"

#include "../FQCommon/IFQRenderObject.h"

namespace fq::game_module
{
	class Particle : public Component
	{
	public:
		Particle();
		~Particle();
	
		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		fq::graphics::IParticleObject* GetParticleObject() const { return mParticleObject; }
		void SetParticleObject(fq::graphics::IParticleObject* val) { mParticleObject = val; }

		fq::graphics::ParticleInfo GetParticleInfomation() const { return mParticleInfomation; }
		void SetParticleInfomation(fq::graphics::ParticleInfo val) { mParticleInfomation = val; }


	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		graphics::IParticleObject* mParticleObject;
		graphics::ParticleInfo mParticleInfomation;
	};

}