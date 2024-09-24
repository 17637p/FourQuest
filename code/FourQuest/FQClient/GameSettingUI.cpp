#include "GameSettingUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"

std::shared_ptr<fq::game_module::Component> fq::client::GameSettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneGameSettingUI = std::dynamic_pointer_cast<GameSettingUI>(clone);

	if (cloneGameSettingUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneGameSettingUI = game_module::ObjectPool::GetInstance()->Assign<GameSettingUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
	// screenManager ���
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
