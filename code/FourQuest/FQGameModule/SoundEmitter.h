#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// 사운드 방출기, 사운드 클립을 참조하여 방출을 처리해줌
	/// </summary>
	class SoundEmitter : public Component
	{
	public:
		SoundEmitter();
		~SoundEmitter();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		virtual void OnUpdate(float dt) override;
		virtual void OnStart() override;

	private:
		entt::meta_handle GetHandle() override;

		void playSound();

	private:
		bool mbIsPlayLoop;
		bool mbIsPlayStateEnter;
		float mSoundTurm;
		bool mbUseRandomPlay;
		std::vector<std::string> mSoundNames;

		float mElapsedTime;
		unsigned int mSoundIndex;
		bool mbIsPlaySoundOnce;
		
		friend void RegisterMetaData();
	};
}