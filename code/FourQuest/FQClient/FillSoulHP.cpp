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

	if (cloneMonster == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneMonster = game_module::ObjectPool::GetInstance()->Assign<FillSoulHP>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

		// ����Ʈ ����
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = soul->GetGameObject();
		stateEvent.RegisterKeyName = "S_Fill_HP";
		GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));

		// ���� ���
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "S_Charge", false , fq::sound::EChannel::SE });
	}
}
