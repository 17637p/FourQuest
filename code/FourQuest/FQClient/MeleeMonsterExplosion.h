#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class MeleeMonsterExplosion : public game_module::Component
	{
	public:
		MeleeMonsterExplosion();
		~MeleeMonsterExplosion();

		/// <summary>
		/// 몬스터가 폭발합니다
		/// 
		/// 폭발은 예술이다
		/// </summary>
		void Explode();

		std::shared_ptr<fq::game_module::GameObject> EmitExplosionWaringUI();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;
		void OnStart()override;

	private:
		float mExplosionRadius;
		float mExplosionDamage;
		float mExplosionTime;

		fq::game_module::PrefabResource mWarningUI;
		fq::game_module::PrefabResource mExplosion;

		friend void RegisterMetaData();
	};


}