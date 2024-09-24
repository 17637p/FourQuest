#include "GameSettingUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"

std::shared_ptr<fq::game_module::Component> fq::client::GameSettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGameSettingUI = std::dynamic_pointer_cast<GameSettingUI>(clone);

	if (cloneGameSettingUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneGameSettingUI = game_module::ObjectPool::GetInstance()->Assign<GameSettingUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneGameSettingUI = *this;
	}

	return cloneGameSettingUI;
}

fq::client::GameSettingUI::GameSettingUI()
{
}

fq::client::GameSettingUI::~GameSettingUI()
{
}

void fq::client::GameSettingUI::OnStart()
{
	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = scene->GetScreenManager();
	mTimeManager = scene->GetTimeManager();
}

void fq::client::GameSettingUI::OnUpdate(float dt)
{
	setScaleScreen();
}

void fq::client::GameSettingUI::setScaleScreen()
{
	// 화면 크기에 따른 Scale 자동 조정 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}
