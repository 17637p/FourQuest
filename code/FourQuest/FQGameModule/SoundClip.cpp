#include "SoundClip.h"

#include "EventManager.h"
#include "Scene.h"
#include "Event.h"

fq::game_module::SoundClip::~SoundClip()
{
}

fq::game_module::SoundClip::SoundClip()
	:mSounds{}
{

}

void fq::game_module::SoundClip::Play(SoundIndex soundIndex, bool bIsLoop, ChannelIndex channelIndex)
{
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mSounds[soundIndex].path, bIsLoop, channelIndex });
}

void fq::game_module::SoundClip::Play(std::string key, bool bIsLoop, ChannelIndex channelIndex)
{
	auto iter = std::lower_bound(mSounds.begin(), mSounds.end(), key, [](const SoundInfo& info, const std::string& key)
		{
			return  info.key < key;
		});

	if (iter != mSounds.end() && iter->key == key)
	{
		int index = std::distance(mSounds.begin(), iter);
		Play(index, bIsLoop, channelIndex);
		spdlog::warn("[SoundClip] playe sound");
	}
	else
	{
		spdlog::warn("[SoundClip] {} can't find key");
	}
}

void fq::game_module::SoundClip::StopChannel(ChannelIndex index)
{
	GetScene()->GetEventManager()->FireEvent<fq::event::OnStopChannel>({ index });
}

entt::meta_handle fq::game_module::SoundClip::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SoundClip::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSoundClip = std::dynamic_pointer_cast<SoundClip>(clone);

	if (cloneSoundClip == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSoundClip = ObjectPool::GetInstance()->Assign<SoundClip>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSoundClip = *this;
	}

	return cloneSoundClip;
}

void fq::game_module::SoundClip::OnStart()
{
	std::sort(mSounds.begin(), mSounds.end(), [](const SoundInfo& lhs, const SoundInfo& rhs)
		{
			return lhs.key < rhs.key;
		});
}

