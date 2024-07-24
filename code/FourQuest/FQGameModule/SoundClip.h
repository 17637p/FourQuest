#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// 사운드 클립
	/// </summary>
	class SoundClip : public Component
	{
	public:
		using SoundPath = std::string;
		struct SoundInfo
		{
			std::string key;
			SoundPath path;
		};
		using SoundIndex = unsigned int;
		using ChannelIndex = unsigned int;
		using SoundContainer = std::vector<SoundInfo>;

	public:
		SoundClip();
		~SoundClip();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 사운드들을 담은 컨테이너를 반환합니다 
		/// </summary>
		SoundContainer GetSounds()const { return mSounds; }

		/// <summary>
		/// 사`	운드들을 설정합니다 
		/// </summary>
		void SetSounds(SoundContainer sounds) { mSounds = sounds; }

		/// <summary>
		/// 사운드를 재생합니다 
		/// </summary>
		/// <param name="index"></param>
		void Play(SoundIndex soundIndex, bool bIsLoop, ChannelIndex channelIndex);

		/// <summary>
		/// 채널을 정지합니다 
		/// </summary>
		void StopChannel(ChannelIndex index);

		/// <summary>
		/// Key 값으로 사운드를 재생합니다 
		/// </summary>
		void Play(std::string key, bool bIsLoop, ChannelIndex channelIndex);

	private:
		entt::meta_handle GetHandle() override;

		void OnStart() override;

	private:
		SoundContainer mSounds;
	};
}