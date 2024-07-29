#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// ���� Ŭ��
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
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ������� ���� �����̳ʸ� ��ȯ�մϴ� 
		/// </summary>
		SoundContainer GetSounds()const { return mSounds; }

		/// <summary>
		/// ��`	������ �����մϴ� 
		/// </summary>
		void SetSounds(SoundContainer sounds) { mSounds = sounds; }

		/// <summary>
		/// ���带 ����մϴ� 
		/// </summary>
		/// <param name="index"></param>
		void Play(SoundIndex soundIndex, bool bIsLoop, ChannelIndex channelIndex);

		/// <summary>
		/// ä���� �����մϴ� 
		/// </summary>
		void StopChannel(ChannelIndex index);

		/// <summary>
		/// Key ������ ���带 ����մϴ� 
		/// </summary>
		void Play(std::string key, bool bIsLoop, ChannelIndex channelIndex);

	private:
		entt::meta_handle GetHandle() override;

		void OnStart() override;

	private:
		SoundContainer mSounds;
	};
}