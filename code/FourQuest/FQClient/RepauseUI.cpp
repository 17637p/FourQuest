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
	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::RepauseUI::OnUpdate(float dt)
{
	SetScaleScreen();

	// UI 조작 (계속하기, 선택 옮기기 등)
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
	// Scale 자동 조정 
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

	if (cloneRepauseUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneRepauseUI = game_module::ObjectPool::GetInstance()->Assign<RepauseUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneRepauseUI = *this;
	}

	return cloneRepauseUI;
}
