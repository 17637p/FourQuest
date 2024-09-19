#pragma once

#include "../FQGameModule/Component.h"

namespace fq::client
{
	class BGM : public fq::game_module::Component
	{
	public:
		BGM();
		~BGM();

		void PlayBGM();

	private:
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;
		entt::meta_handle GetHandle() override { return *this; }

		void OnStart()override;
	private:
		std::string mSoundKey;
		friend void RegisterMetaData();
	};


}