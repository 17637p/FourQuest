#include "SettingUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"

fq::client::SettingUI::SettingUI()
	:mScreenManager(nullptr)
{

}

fq::client::SettingUI::~SettingUI()
{
}

void fq::client::SettingUI::OnStart()
{
	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::SettingUI::OnUpdate(float dt)
{
	SetScaleScreen();
}

std::shared_ptr<fq::game_module::Component> fq::client::SettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSettingUI = std::dynamic_pointer_cast<SettingUI>(clone);

	if (cloneSettingUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSettingUI = game_module::ObjectPool::GetInstance()->Assign<SettingUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSettingUI = *this;
	}

	return cloneSettingUI;
}

void fq::client::SettingUI::SetScaleScreen()
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

//GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
