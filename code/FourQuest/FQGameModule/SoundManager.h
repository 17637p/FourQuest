#pragma once
#pragma once

#include <unordered_map>
#include <string>
#include "Fmod/fmod.hpp"
#include "SoundEnum.h"


namespace fq::game_module
{
	/// <summary>
	/// ���带 �����ϴ� �Ŵ��� Ŭ����
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
		/// ���� �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize();

		/// <summary>
		/// ���� �ý����� ������Ʈ�մϴ� 
		/// </summary>
		void Update();

		/// <summary>
		/// ���� �Ŵ����� �����մϴ� 
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���带 �ε��մϴ� 
		/// </summary>
		void LoadSound(const SoundKey& key , const SoundPath& path);

		/// <summary>
		/// ���带 ��ε��մϴ� 
		/// </summary>
		void UnloadSound( const SoundKey& key);

		/// <summary>
		/// �����带 ��ε��մϴ� 
		/// </summary>
		void UnloadAllSound();
		
		/// <summary>
		/// ä���� ����� �����մϴ� 
		/// </summary>
		void StopChannel(ChannelIndex index);

		/// <summary>
		/// ���带 ����մϴ� 
		/// </summary>
		void Play(const SoundKey& key, bool bIsLoop, ChannelIndex index);

		/// <summary>
		/// ä���� ������ �����մϴ�
		/// ������ 0.f ~ 1.f
		/// </summary>
		void SetChannelVoulme(fq::sound::EChannel channel, float voulme);

		/// <summary>
		/// ä���� ������ ��ȯ�մϴ�
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