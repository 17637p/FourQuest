#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class PlayerDummy : public fq::game_module::Component
	{
	public:
		PlayerDummy();
		~PlayerDummy();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		void OnStart() override;
		void OnUpdate(float dt) override;

		entt::meta_handle GetHandle() override { return *this; }

		void SetDestroyDuration(float duration) { mDuration = duration; }

	private:
		float mElapsedTime;
		float mDuration;

		friend void RegisterMetaData();
	};
}