#pragma once
#pragma once

#include <unordered_map>
#include <string>
#include "Fmod/fmod.hpp"
#include "SoundEnum.h"


namespace fq::game_module
{
	/// <summary>
	/// 사운드를 관리하는 매니져 클래스
	/// </summary>
	class SoundManager
	{
		using SoundKey = std::string;
		using SoundPath = std::string;
		using SoundList = std::unordered_map<SoundKey, FMOD::Sound*>;
		using SoundSystem = FMOD::System;
		using SoundChannel = FMOD::Channel;
		using ChannelIndex = unsigned int;


	public:
		SoundManager();
		~SoundManager();

	public:
		/// <summary>
		/// 사운드 시스템을 초기화합니다
		/// </summary>
		void Initialize();

		/// <summary>
		/// 사운드 시스템을 업데이트합니다 
		/// </summary>
		void Update();

		/// <summary>
		/// 사운드 매니져를 종료합니다 
		/// </summary>
		void Finalize();

		/// <summary>
		/// 사운드를 로드합니다 
		/// </summary>
		void LoadSound(const SoundKey& key , const SoundPath& path);

		/// <summary>
		/// 사운드를 언로드합니다 
		/// </summary>
		void UnloadSound( const SoundKey& key);

		/// <summary>
		/// 모든사운드를 언로드합니다 
		/// </summary>
		void UnloadAllSound();
		
		/// <summary>
		/// 채널의 재생을 정지합니다 
		/// </summary>
		void StopChannel(ChannelIndex index);

		/// <summary>
		/// 사운드를 재생합니다 
		/// </summary>
		void Play(const SoundKey& key, bool bIsLoop, ChannelIndex index);

		/// <summary>
		/// 채널의 볼륨을 설정합니다
		/// 범위는 0.f ~ 1.f
		/// </summary>
		void SetChannelVoulme(fq::sound::EChannel channel, float voulme);

		/// <summary>
		/// 채널의 볼륨을 반환합니다
		/// </summary>
		float GetChannelVoulme(fq::sound::EChannel channel)const;

	private:
		static constexpr ChannelIndex MaxChannel = 32;

		FMOD_RESULT mFmodResult;
		SoundSystem* mSoundSystem;
		SoundChannel* mChannel[MaxChannel];
		float mChannelVolume[MaxChannel];
		SoundList mSoundList;
		unsigned int mVersion;
	};


}