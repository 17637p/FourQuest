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

	if (cloneBGM == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneBGM = game_module::ObjectPool::GetInstance()->Assign<BGM>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
