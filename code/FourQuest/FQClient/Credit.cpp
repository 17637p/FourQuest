#include "Credit.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/VideoUI.h"

fq::client::Credit::Credit()
	:mDelaySeconds(0),
	mNextSceneName("")
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

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::B) == EKeyState::Hold)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ mNextSceneName, true });
		}
	}

	if (GetComponent<game_module::VideoUI>()->IsEndPlay(mDelaySeconds))
	{
		GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ mNextSceneName, true });
	}

	for (int i = 0; i < 4; ++i)
	{
		if (input->IsPadKeyState(i, EPadKey::Start, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "PvP", true});
		}
	}
}

void fq::client::Credit::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();
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

