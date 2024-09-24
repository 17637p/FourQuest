#include "ResetPopupUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"

#include "ClientEvent.h"

fq::client::ResetPopupUI::ResetPopupUI()
	:mScreenManager(nullptr)
{
}

void fq::client::ResetPopupUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::ResetPopupUI::OnUpdate(float dt)
{
	SetScaleScreen();

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupReset>({false});
			GetScene()->DestroyGameObject(GetGameObject());
		}

		if (input->IsPadKeyState(i, EPadKey::A, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupReset>({ true });
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}

fq::client::ResetPopupUI::~ResetPopupUI()
{
}

void fq::client::ResetPopupUI::SetScaleScreen()
{
	// Scale �ڵ� ���� 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}

std::shared_ptr<fq::game_module::Component> fq::client::ResetPopupUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneResetUI = std::dynamic_pointer_cast<ResetPopupUI>(clone);

	if (cloneResetUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneResetUI = game_module::ObjectPool::GetInstance()->Assign<ResetPopupUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneResetUI = *this;
	}

	return cloneResetUI;
}
