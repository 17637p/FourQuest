#include "BGM.h"

#include "../FQGameModule/GameModule.h"

fq::client::BGM::BGM()
{

}

fq::client::BGM::~BGM()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::BGM::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto cloneBGM = std::dynamic_pointer_cast<BGM>(clone);

	if (cloneBGM == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneBGM = game_module::ObjectPool::GetInstance()->Assign<BGM>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneBGM = *this;
	}

	return cloneBGM;
}

void fq::client::BGM::OnStart()
{
	PlayBGM();
}

void fq::client::BGM::PlayBGM()
{
	GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ mSoundKey, true , 0 });
}
