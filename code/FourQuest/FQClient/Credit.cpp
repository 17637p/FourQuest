#include "Credit.h"

fq::client::Credit::Credit()
	:mSeconds(0),
	mNextSceneName(""),
	mCurTime(0)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::Credit::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCredit = std::dynamic_pointer_cast<Credit>(clone);

	if (cloneCredit == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCredit = game_module::ObjectPool::GetInstance()->Assign<Credit>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCredit = *this;
	}

	return cloneCredit;
}

fq::client::Credit::~Credit()
{
}

void fq::client::Credit::OnUpdate(float dt)
{
	mCurTime += dt;
	if (mCurTime > mSeconds)
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ mNextSceneName, true });
	}

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::B) == EKeyState::Hold)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ mNextSceneName, true });
		}
	}
}

void fq::client::Credit::OnStart()
{
	mCurTime = 0;
}

