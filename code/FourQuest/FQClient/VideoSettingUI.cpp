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
	:mIsActive(true),
	mHaveToDestroyed(false),
	mResetMessagePrefab(),
	mSaveMessagePrefab(),
	mSettingUIPrefab(),
	mOffPopupResetHandler(),
	mOffPopupSaveHandler(),
	mScreenManager(nullptr),
	mTimeManager(nullptr),
	mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mSelectButtonBackground(nullptr),
	mSelectButtonSoulIcon(nullptr),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(1000),
	mExplanationTexts(),
	mExplanationTextUI(nullptr),
	mRadioOnImage(),
	mIsWindow(true),
	mResolutionTextUI(nullptr),
	mResolutionIndex(0),
	mResolutions(),
	mSaveIsWindow(true),
	mSaveResolution()
{
}

fq::client::VideoSettingUI::VideoSettingUI(const VideoSettingUI& other)
	:mIsActive(true),
	mHaveToDestroyed(false),
	mResetMessagePrefab(other.mResetMessagePrefab),
	mSaveMessagePrefab(other.mSaveMessagePrefab),
	mSettingUIPrefab(other.mSettingUIPrefab),
	mOffPopupResetHandler(),
	mOffPopupSaveHandler(),
	mScreenManager(nullptr),
	mTimeManager(nullptr),
	mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mSelectButtonBackground(nullptr),
	mSelectButtonSoulIcon(nullptr),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(other.mUIAnimSpeed),
	mExplanationTexts(),
	mExplanationTextUI(nullptr),
	mRadioOnImage(),
	mIsWindow(true),
	mResolutionTextUI(nullptr),
	mResolutionIndex(0),
	mResolutions(),
	mSaveIsWindow(true),
	mSaveResolution()

{
}

fq::client::VideoSettingUI& fq::client::VideoSettingUI::operator=(const VideoSettingUI& other)
{
	mSelectButtonID = 0;
	mStickDelay = other.mStickDelay;
	mUIAnimSpeed = other.mUIAnimSpeed;
	mIsActive = other.mIsActive;
	mResetMessagePrefab = other.mResetMessagePrefab;
	mSaveMessagePrefab = other.mSaveMessagePrefab;
	mSettingUIPrefab = other.mSettingUIPrefab;

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
	mExplanationTexts.push_back(wstringToString(L"전체화면과 창 모드 중 선택합니다."));

	mResolutions.push_back({1920, 1080});
	mResolutions.push_back({1600, 900});
	mResolutions.push_back({1280, 720});
	mResolutions.push_back({1024, 576});

	mSaveIsWindow = !mScreenManager->IsFullScreen();
	mSaveResolution.x = mScreenManager->GetScreenWidth();
	mSaveResolution.y = mScreenManager->GetScreenHeight();
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

void fq::client::VideoSettingUI::setResolutionTextUI()
{
	std::string resolutionText = std::to_string((int)mResolutions[mResolutionIndex].x) + " X " + std::to_string((int)mResolutions[mResolutionIndex].y);
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

	DirectX::SimpleMath::Vector3 curPosition = mSelectBackground->GetComponent<game_module::Transform>()->GetWorldPosition();

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

	mSelectBackground->GetComponent<game_module::Transform>()->SetWorldPosition(curPosition);
}

void fq::client::VideoSettingUI::setRadio()
{
	mRadioOnImage[0]->SetIsRender(0, !mIsWindow);
	mRadioOnImage[1]->SetIsRender(0, mIsWindow);
}

void fq::client::VideoSettingUI::processInput(float dt)
{
	mCurStickDelay += dt;

	// UI 조작 (계속하기, 선택 옮기기 등)
	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		// Stick Y 값
		float isLeftStickY = input->GetStickInfomation(i, EPadStick::leftY);

		// 아래로
		bool isDpadDownTap = input->GetPadKeyState(i, EPadKey::DpadDown) == EKeyState::Tap;
		// Stick 처리
		bool isLeftStickDownTap = false;
		if (isLeftStickY < -0.9f)
		{
			if (mCurStickDelay > mStickDelay)
			{
				mCurStickDelay = 0;
				isLeftStickDownTap = true;
			}
		}

		if (isDpadDownTap || isLeftStickDownTap)
		{
			if (mSelectButtonID == 2)
			{
				mSelectButtonID++;
			}
			if (mSelectButtonID < 2)
			{
				mSelectButtonID++;
				mExplanationTextUI->SetText(mExplanationTexts[mSelectButtonID]);
			}
		}

		// 위로
		bool isDpadUpTap = input->GetPadKeyState(i, EPadKey::DpadUp) == EKeyState::Tap;
		// Stick 처리
		bool isLeftStickUpTap = false;
		if (isLeftStickY > 0.9f)
		{
			if (mCurStickDelay > mStickDelay)
			{
				mCurStickDelay = 0;
				isLeftStickUpTap = true;
			}
		}

		if (isDpadUpTap || isLeftStickUpTap)
		{
			if (mSelectButtonID > 0)
			{
				mSelectButtonID--;
				mExplanationTextUI->SetText(mExplanationTexts[mSelectButtonID]);
			}
		}

		// Stick X 값
		float isLeftStickX = input->GetStickInfomation(i, EPadStick::leftX);

		// 좌로
		bool isDpadLeftTap = input->GetPadKeyState(i, EPadKey::DpadLeft) == EKeyState::Tap;
		// Stick 처리
		bool isLeftStickLeftTap = false;
		if (isLeftStickX < -0.9f)
		{
			if (mCurStickDelay > mStickDelay)
			{
				mCurStickDelay = 0;
				isLeftStickLeftTap = true;
			}
		}

		if (isDpadLeftTap || isLeftStickLeftTap)
		{
			mResolutionIndex--;
			if (mResolutionIndex <= -1)
			{
				mResolutionIndex = mResolutions.size() - 1;
			}
			setResolutionTextUI();
		}

		// 우로
		bool isDpadRightTap = input->GetPadKeyState(i, EPadKey::DpadRight) == EKeyState::Tap;
		// Stick 처리
		bool isLeftStickRightTap = false;
		if (isLeftStickX > 0.9f)
		{
			if (mCurStickDelay > mStickDelay)
			{
				mCurStickDelay = 0;
				isLeftStickRightTap = true;
			}
		}

		if (isDpadRightTap || isLeftStickRightTap)
		{
			mResolutionIndex++;
			if (mResolutionIndex >= mResolutions.size())
			{
				mResolutionIndex = 0;
			}
			setResolutionTextUI();
		}
	}

	for (int i = 0; i < 4; i++)
	{
		// A Button
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap)
		{
			if (mSelectButtonID == 3)
			{
				saveSettingData();
			}
			else if (mSelectButtonID == 1)
			{
				mIsWindow = false;
				setRadio();
			}
			else if (mSelectButtonID == 2)
			{
				mIsWindow = true;
				setRadio();
			}
		}
		// B Button
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			if (isChangeSettingData())
			{
				mIsActive = false;
				spawnUIObject(mSaveMessagePrefab);
			}
			else
			{
				spawnUIObject(mSettingUIPrefab);
				GetScene()->DestroyGameObject(GetGameObject());
			}
		}
		// Y Button
		if (input->IsPadKeyState(i, EPadKey::Y, EKeyState::Tap))
		{
			mIsActive = false;
			spawnUIObject(mResetMessagePrefab);
		}
	}
}

void fq::client::VideoSettingUI::initGameSetting()
{
	mIsWindow = true;
	mResolutionIndex = 0;
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
	// 창모드 비교
	if (mSaveIsWindow != mIsWindow)
	{
		return true;
	}
	else
	{
		// 해상도 비교
		if (mSaveResolution.x != mResolutions[mResolutionIndex].x)
		{
			return true;
		}
	}

	return false;
}

void fq::client::VideoSettingUI::saveSettingData()
{
	mSaveIsWindow = mIsWindow;
	mSaveResolution = mResolutions[mResolutionIndex];

	mScreenManager->SetScreenSize(mResolutions[mResolutionIndex].x, mResolutions[mResolutionIndex].y);
	mScreenManager->SetFullScreen(!mIsWindow);
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

void fq::client::VideoSettingUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupResetHandler);
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupSaveHandler);
}

