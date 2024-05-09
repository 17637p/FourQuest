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

fq::game_module::Component* fq::game_module::SoundClip::Clone(Component* clone /* = nullptr */) const
{
	SoundClip* soundClip = static_cast<SoundClip*>(clone);

	if (soundClip == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		soundClip = new SoundClip(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*soundClip = *this;
	}

	return soundClip;
}

