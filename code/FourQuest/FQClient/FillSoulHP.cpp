#include "FillSoulHP.h"
#include "Soul.h"

fq::client::FillSoulHP::FillSoulHP()
	: mMinHP(10.f)
	, mFillHP(100.f)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::FillSoulHP::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<FillSoulHP> cloneMonster = std::dynamic_pointer_cast<FillSoulHP>(clone);

	if (cloneMonster == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<FillSoulHP>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneMonster = *this;
	}

	return cloneMonster;
}

void fq::client::FillSoulHP::OnTriggerStay(const game_module::Collision& collision)
{
	auto soul = collision.other->GetComponent<Soul>();

	if (soul == nullptr)
	{
		return;
	}

	if (soul->GetHP() < mMinHP)
	{
		soul->SetHP(mFillHP);

		// 이펙트 방출
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = soul->GetGameObject();
		stateEvent.RegisterKeyName = "S_Fill_HP";
		GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));

		// 사운드 재생
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "S_Charge", false , fq::sound::EChannel::SE });
	}
}
