#pragma once

#include <unordered_map>
#include <string>

#include "Fmod/fmod.hpp"

namespace fq::game_module
{
	/// <summary>
	/// ���带 �����ϴ� �Ŵ��� Ŭ����
	/// </summary>
	class SoundManager
	{
		using SoundPath = std::string;
		using SoundList = std::unordered_map<SoundPath, FMOD::Sound*>;
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
		void LoadSound(const SoundPath& path);

		/// <summary>
		/// ���带 ��ε��մϴ� 
		/// </summary>
		void UnloadSound(const SoundPath& path);

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
		void Play(const SoundPath& path, bool bIsLoop, ChannelIndex index);

	private:
		static constexpr ChannelIndex MaxChannel = 32;

		FMOD_RESULT mFmodResult;
		SoundSystem* mSoundSystem;
		SoundChannel* mChannel[MaxChannel];
		SoundList mSoundList;
		unsigned int mVersion;
	};


}