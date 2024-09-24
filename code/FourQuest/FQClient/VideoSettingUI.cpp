#include "VideoSettingUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/SpriteAnimationUI.h"

#include "ClientEvent.h"
#include "SettingVariable.h"

#include <boost/locale.hpp>

std::shared_ptr<fq::game_module::Component> fq::client::VideoSettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneVideoSettingUI = std::dynamic_pointer_cast<VideoSettingUI>(clone);

	if (cloneVideoSettingUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneVideoSettingUI = game_module::ObjectPool::GetInstance()->Assign<VideoSettingUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneVideoSettingUI = *this;
	}

	return cloneVideoSettingUI;
}

fq::client::VideoSettingUI::VideoSettingUI()
{
}

fq::client::VideoSettingUI::VideoSettingUI(const VideoSettingUI& other)
{
}

fq::client::VideoSettingUI& fq::client::VideoSettingUI::operator=(const VideoSettingUI& other)
{
	return *this;
}

fq::client::VideoSettingUI::~VideoSettingUI()
{
}

void fq::client::VideoSettingUI::OnStart()
{
	mExplanationTexts.clear();
	mButtons.clear();
	mResolutions.clear();
	mRadioOnImage.clear();

	mIsActive = true;
	mHaveToDestroyed = false;

	// Event 등록
	eventProcessOffPopupReset();
	eventProcessOffPopupSave();

	// Manager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = scene->GetScreenManager();
	mTimeManager = scene->GetTimeManager();

	// Button Set
	auto buttons = GetGameObject()->GetChildren()[4]->GetChildren();
	for (int i = 0; i < 3; i++)
	{
		mButtons.push_back(buttons[i]);
	}
	mRadioOnImage.push_back(mButtons[1]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	mRadioOnImage.push_back(mButtons[2]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());

	mResolutionTextUI = mButtons[0]->GetChildren()[1]->GetComponent<game_module::TextUI>();

	setRadio();

	mSelectButtonID = 0;
	mSelectBackground = GetGameObject()->GetChildren()[0];

	mSelectButtonBackground = GetGameObject()->GetChildren()[3]->GetChildren()[0]->GetComponent<game_module::ImageUI>();
	mSelectButtonSoulIcon = GetGameObject()->GetChildren()[3]->GetChildren()[1]->GetComponent<game_module::SpriteAnimationUI>();

	// Text 넣기
	mExplanationTextUI = GetGameObject()->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"게임의 해상도를 변경합니다. 16:9 화면비를 지원합니다."));
	mExplanationTexts.push_back(wstringToString(L"전체화면과 창 모드 중 선택합니다."));

	mResolutions.push_back({1920, 1080});
	mResolutions.push_back({1600, 900});
	mResolutions.push_back({1280, 720});
	mResolutions.push_back({1024, 576});
}

void fq::client::VideoSettingUI::OnUpdate(float dt)
{
	if (mHaveToDestroyed)
	{
		GetScene()->DestroyGameObject(GetGameObject());
		return;
	}

	float deltaTime = mTimeManager->GetDeltaTime();
	setScaleScreen();
	setSelectBoxPosition(deltaTime);

	if (mIsActive)
	{
		processInput(deltaTime);
	}
}

void fq::client::VideoSettingUI::setScaleScreen()
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

void fq::client::VideoSettingUI::setResolutionTextUI(int width, int height)
{
	std::string resolutionText = std::to_string(width) + " X " + std::to_string(height);
	mResolutionTextUI->SetText(resolutionText);
}

void fq::client::VideoSettingUI::setSelectBoxPosition(float dt)
{
	bool isSelectAcceptButton = false;
	if (mSelectButtonID == 3)
	{
		isSelectAcceptButton = true;
	}

	mSelectButtonBackground->SetIsRender(0, isSelectAcceptButton);
	mSelectButtonSoulIcon->SetIsRender(isSelectAcceptButton);

	mExplanationTextUI->SetIsRender(!isSelectAcceptButton);
	mSelectBackground->GetComponent<game_module::ImageUI>()->SetIsRender(0, !isSelectAcceptButton);
	mSelectBackground->GetChildren()[0]->GetComponent<game_module::SpriteAnimationUI>()->SetIsRender(!isSelectAcceptButton);

	if (mSelectButtonID == 3)
	{
		return;
	}

	// 선택UI 위치로 SelectBox 옮기기 
	game_module::Transform* selectTransform = mButtons[mSelectButtonID]->GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 selectPosition = selectTransform->GetWorldPosition();

	DirectX::SimpleMath::Vector3 curPosition = mSelectBackground->GetComponent<game_module::Transform>()->GetLocalPosition();

	float dist = selectPosition.y - curPosition.y;
	if (std::abs(dist) < mUIAnimSpeed * dt)
	{
		curPosition = selectPosition;
	}
	else if (dist > 0)
	{
		curPosition.y += mUIAnimSpeed * dt;
	}
	else
	{
		curPosition.y -= mUIAnimSpeed * dt;
	}

	curPosition.x = 860;
	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(curPosition);
}

void fq::client::VideoSettingUI::setRadio()
{
	mRadioOnImage[0]->SetIsRender(0, !mIsWindow);
	mRadioOnImage[1]->SetIsRender(0, mIsWindow);
}

void fq::client::VideoSettingUI::processInput(float dt)
{

}

void fq::client::VideoSettingUI::initGameSetting()
{

}

void fq::client::VideoSettingUI::spawnUIObject(fq::game_module::PrefabResource prefab)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
}

bool fq::client::VideoSettingUI::isChangeSettingData()
{
	// 스크린 매니저에서 가져와서
	// 해상도 비교

	// 창모드 비교
	return false;
}

void fq::client::VideoSettingUI::saveSettingData()
{

}

void fq::client::VideoSettingUI::eventProcessOffPopupReset()
{
	mOffPopupResetHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupReset>
		(
			[this](const client::event::OffPopupReset& event)
			{
				mIsActive = true;
				if (event.isReset)
				{
					initGameSetting();
				}
			}
		);
}

void fq::client::VideoSettingUI::eventProcessOffPopupSave()
{
	mOffPopupSaveHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupSave>
		(
			[this](const client::event::OffPopupSave& event)
			{
				mIsActive = true;
				if (event.isSave)
				{
					saveSettingData();

					spawnUIObject(mSettingUIPrefab);
					mHaveToDestroyed = true;
				}
			}
		);
}

std::string fq::client::VideoSettingUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

