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
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mSounds[soundIndex], bIsLoop, channelIndex });
}

void fq::game_module::SoundClip::StopChannel(ChannelIndex index)
{
	GetScene()->GetEventManager()->FireEvent<fq::event::OnStopChannel>({ index });
}

entt::meta_handle fq::game_module::SoundClip::GetHandle()
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SoundClip::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto cloneSoundClip = std::dynamic_pointer_cast<SoundClip>(clone);

	if (cloneSoundClip == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSoundClip = ObjectPool::GetInstance()->Assign<SoundClip>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		*cloneSoundClip = *this;
	}

	return cloneSoundClip;
}

