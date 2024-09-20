#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::client
{
	class DebuffPoisonZone : public fq::game_module::Component
	{
	public:
		DebuffPoisonZone();
		~DebuffPoisonZone();

		float GetPosionDamage() const { return mPoisonDamage; }
		void SetPosionDamage(float damage) { mPoisonDamage = damage; }
		float GetPoisonTurm() const { return mPoisonTurm; }
		void SetPoisonTurm(float turm) { mPoisonTurm = turm; }
		float GetDurationTime() const { return mDurationTime; }
		void SetDurationTime(float time) { mDurationTime = time; }

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mPoisonDamage;
		float mPoisonTurm;
		float mDurationTime;
	};
}

