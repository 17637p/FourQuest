#include "SavePopupUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"

#include "ClientEvent.h"

fq::client::SavePopupUI::SavePopupUI()
	:mScreenManager(nullptr)
{
}

void fq::client::SavePopupUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::SavePopupUI::OnUpdate(float dt)
{
	SetScaleScreen();

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupSave>({false});
			GetScene()->DestroyGameObject(GetGameObject());
		}

		if (input->IsPadKeyState(i, EPadKey::A, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupSave>({ true });
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}

fq::client::SavePopupUI::~SavePopupUI()
{
}

void fq::client::SavePopupUI::SetScaleScreen()
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

std::shared_ptr<fq::game_module::Component> fq::client::SavePopupUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSaveUI = std::dynamic_pointer_cast<SavePopupUI>(clone);

	if (cloneSaveUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSaveUI = game_module::ObjectPool::GetInstance()->Assign<SavePopupUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSaveUI = *this;
	}

	return cloneSaveUI;
}
