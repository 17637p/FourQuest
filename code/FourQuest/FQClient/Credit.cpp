#include "Credit.h"

#include "../FQGameModule/Transform.h"

fq::client::Credit::Credit()
	:mSeconds(0),
	mNextSceneName(""),
	mCurTime(0)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::Credit::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCredit = std::dynamic_pointer_cast<Credit>(clone);

	if (cloneCredit == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCredit = game_module::ObjectPool::GetInstance()->Assign<Credit>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCredit = *this;
	}

	return cloneCredit;
}

fq::client::Credit::~Credit()
{
}

void fq::client::Credit::OnUpdate(float dt)
{
	setScaleScreen();

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
	mScreenManager = GetScene()->GetScreenManager();

	mCurTime = 0;
}

void fq::client::Credit::setScaleScreen()
{
	// ȭ�� ũ�⿡ ���� Scale �ڵ� ���� 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}

