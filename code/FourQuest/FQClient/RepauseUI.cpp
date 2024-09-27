#include "RepauseUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"

#include "ClientEvent.h"

fq::client::RepauseUI::RepauseUI()
	:mScreenManager(nullptr)
{
}

fq::client::RepauseUI::~RepauseUI()
{
}

void fq::client::RepauseUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::RepauseUI::OnUpdate(float dt)
{
	SetScaleScreen();

	// UI ���� (����ϱ�, ���� �ű�� ��)
	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupRepause>({});
			GetScene()->DestroyGameObject(GetGameObject());
		}

		if (input->IsPadKeyState(i, EPadKey::A, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "TitleUI", true });
		}
	}
}

void fq::client::RepauseUI::SetScaleScreen()
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

std::shared_ptr<fq::game_module::Component> fq::client::RepauseUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneRepauseUI = std::dynamic_pointer_cast<RepauseUI>(clone);

	if (cloneRepauseUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneRepauseUI = game_module::ObjectPool::GetInstance()->Assign<RepauseUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneRepauseUI = *this;
	}

	return cloneRepauseUI;
}
