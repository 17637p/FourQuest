#pragma once
#include "Component.h"

namespace fq::game_module
{
	/// <summary>
	/// ���� Ŭ��
	/// </summary>
	class SoundClip : public Component
	{
		using SoundIndex = unsigned int;
		using ChannelIndex = unsigned int;
		using SoundPath = std::string;
		using SoundContainer = std::vector<SoundPath>;
	public:
		SoundClip();
		~SoundClip();

		/// <summary>
		/// ���纻�� �Ҵ��ؼ� ��ȯ�մϴ� 
		/// </summary>
		/// <param name="clone"></param>
		Component* Clone(Component* clone /* = nullptr */)const override;

		/// <summary>
		/// ������� ���� �����̳ʸ� ��ȯ�մϴ� 
		/// </summary>
		SoundContainer GetSounds()const { return mSounds; }

		/// <summary>
		/// ������� �����մϴ� 
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

	private:
		entt::meta_handle GetHandle() override;

	private:
		SoundContainer mSounds;
	};
}