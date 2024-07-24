#include "SoundManager.h"

#include <assert.h>
#include <spdlog/spdlog.h>

fq::game_module::SoundManager::SoundManager()
	:mFmodResult{}
	, mSoundSystem(nullptr)
	, mChannel{}
	, mVersion{}
	, mSoundList{}
{}

fq::game_module::SoundManager::~SoundManager()
{}

void fq::game_module::SoundManager::Update()
{
	mSoundSystem->update();
}

void fq::game_module::SoundManager::Initialize()
{
	mFmodResult = FMOD::System_Create(&mSoundSystem);
	assert(mFmodResult == FMOD_OK);

	mFmodResult = mSoundSystem->getVersion(&mVersion);
	assert(mFmodResult == FMOD_OK);

	mFmodResult = mSoundSystem->init(MaxChannel, FMOD_INIT_NORMAL, nullptr);
	assert(mFmodResult == FMOD_OK);
}

void fq::game_module::SoundManager::Finalize()
{
	UnloadAllSound();

	mSoundSystem->close();
	mSoundSystem->release();
}


void fq::game_module::SoundManager::LoadSound(const SoundKey& key, const SoundPath& path)
{
	if (mSoundList.find(key) != mSoundList.end()) return;

	FMOD::Sound* sound = nullptr;

	mFmodResult = mSoundSystem->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);

	if (mFmodResult != FMOD_OK)
	{
		sound->release();
		spdlog::warn("[SoundManager]  \"{}\" load failed", path.c_str());
		return;
	}

	mSoundList.insert({ key, sound });
}

void fq::game_module::SoundManager::UnloadAllSound()
{
	for (unsigned int index = 0; index < MaxChannel; ++index)
	{
		StopChannel(index);
	}

	for (auto& sound : mSoundList)
	{
		sound.second->release();
	}

	mSoundList.clear();
}


void fq::game_module::SoundManager::UnloadSound(const SoundKey& key)
{
		auto iter = mSoundList.find(key);

		if (iter == mSoundList.end())
		{
			return;
		}

		auto sound = iter->second;
		sound->release();
		mSoundList.erase(iter);
}

void fq::game_module::SoundManager::StopChannel(ChannelIndex index)
{
	assert(MaxChannel > index);

	if (mChannel[index])
	{
		mChannel[index]->stop();
	}
}

void fq::game_module::SoundManager::Play(const SoundKey& key, bool bIsLoop, ChannelIndex index)
{
	auto iter = mSoundList.find(key);

	if (iter == mSoundList.end())
	{
		SPDLOG_WARN("{} can't found sound ", key);
		return;
	}

	mFmodResult = mSoundSystem->playSound(iter->second, nullptr, false, &mChannel[index]);

	if (mFmodResult == FMOD_OK)
	{
		bIsLoop ? mChannel[index]->setMode(FMOD_LOOP_NORMAL) : mChannel[index]->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		SPDLOG_WARN(" play sound failed", key);
	}
}

