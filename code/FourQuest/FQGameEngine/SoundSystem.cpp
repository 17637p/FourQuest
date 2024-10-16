#include "SoundSystem.h"

#include "GameProcess.h"
#include "../FQGameModule/SoundClip.h"

void fq::game_engine::SoundSystem::Initialize(fq::game_engine::GameProcess* game)
{
	mEventManager = game->mEventManager.get();
	mSoundManager = game->mSoundManager.get();
	mScene = game->mSceneManager->GetCurrentScene();

	mOnLoadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnLoadScene>(this, &SoundSystem::OnLoadScene);

	mOnUnloadSceneHandler = mEventManager->
		RegisterHandle<fq::event::OnUnloadScene>(this, &SoundSystem::OnUnLoadScene);

	mOnStopChannelHandler = mEventManager->
		RegisterHandle<fq::event::OnStopChannel>(this, &SoundSystem::OnStopChannel);

	mOnPlaySoundHandler = mEventManager->
		RegisterHandle<fq::event::OnPlaySound>(this, &SoundSystem::OnPlaySound);
}

void fq::game_engine::SoundSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
	mScene->ViewComponents<fq::game_module::SoundClip>(
		[this](fq::game_module::GameObject& object, fq::game_module::SoundClip& clip)
		{
			auto sounds = clip.GetSounds();
			for (auto& sound : sounds)
			{
				mSoundManager->LoadSound(sound.key, sound.path);
			}
		}
	);

	mbIsGameLoaded = true;
}

void fq::game_engine::SoundSystem::OnUnLoadScene()
{
	mSoundManager->StopChannelsWithoutNoneStopChannel();
	mbIsGameLoaded = false;
}

void fq::game_engine::SoundSystem::OnPlaySound(fq::event::OnPlaySound event)
{
	mSoundManager->Play(event.key, event.bIsLoop, event.channelIndex);
}

void fq::game_engine::SoundSystem::OnStopChannel(fq::event::OnStopChannel event)
{
	mSoundManager->StopChannel(event.channelIndex);
}

void fq::game_engine::SoundSystem::Finalize()
{
	mSoundManager->UnloadAllSound();
}
