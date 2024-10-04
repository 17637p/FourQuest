#include "GameSettingUI.h"

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
	mExplanationTexts(0),
	mExplanationTextUI(nullptr),
	mIsOn(),
	mToggleOnImage(),
	mSaveISOns()
{
}

fq::client::GameSettingUI::GameSettingUI(const GameSettingUI& other)
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
	mExplanationTexts(0),
	mExplanationTextUI(nullptr),
	mIsOn(),
	mToggleOnImage(),
	mSaveISOns()
{
}

fq::client::GameSettingUI& fq::client::GameSettingUI::operator=(const GameSettingUI& other)
{
	mSelectButtonID = other.mSelectButtonID;
	mStickDelay = other.mStickDelay;
	mUIAnimSpeed = other.mUIAnimSpeed;
	mIsActive = other.mIsActive;
	mResetMessagePrefab = other.mResetMessagePrefab;
	mSaveMessagePrefab = other.mSaveMessagePrefab;
	mSettingUIPrefab = other.mSettingUIPrefab;

	return *this;
}

fq::client::GameSettingUI::~GameSettingUI()
{
}

void fq::client::GameSettingUI::OnStart()
{
	mExplanationTexts.clear();
	mButtons.clear();
	mIsOn.clear();
	mToggleOnImage.clear();
	mSaveISOns.clear();

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
		mToggleOnImage.push_back(mButtons[i]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
	}

	mIsOn.push_back(SettingVariable::IsVibe);
	mIsOn.push_back(SettingVariable::IsUsedAimAssist);
	mIsOn.push_back(SettingVariable::IsAllowOtherPlayerAttack);

	mSaveISOns = mIsOn;

	for (int i = 0; i < 3; i++)
	{
		setToggle(i);
	}

	mSelectButtonID = 0;
	mSelectBackground = GetGameObject()->GetChildren()[0];

	mSelectButtonBackground = GetGameObject()->GetChildren()[3]->GetChildren()[0]->GetComponent<game_module::ImageUI>();
	mSelectButtonSoulIcon = GetGameObject()->GetChildren()[3]->GetChildren()[1]->GetComponent<game_module::SpriteAnimationUI>();

	// Text 넣기
	mExplanationTextUI = GetGameObject()->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"컨트롤러의 진동 기능을 켜거나 끕니다. 더욱 몰입감 있는 플레이가 가능합니다."));
	mExplanationTexts.push_back(wstringToString(L"공격 시 사정거리 내의 적을 자동으로 조준합니다.\n초보자나 조준이 어려운 플레이어에게 추천합니다."));
	mExplanationTexts.push_back(wstringToString(L"플레이어의 공격이 다른 플레이어에게 영향을 미칠 수 있는지를 설정합니다.\n실수로 아군에게 피해를 줄 수 있으니 주의가 필요합니다."));
}

void fq::client::GameSettingUI::OnUpdate(float dt)
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

	auto myPos = myTransform->GetLocalPosition();
	myPos.y = 120 * scaleY;
	myTransform->SetLocalPosition(myPos);
}

void fq::client::GameSettingUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupResetHandler);
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupSaveHandler);
}

void fq::client::GameSettingUI::eventProcessOffPopupReset()
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

void fq::client::GameSettingUI::eventProcessOffPopupSave()
{
	mOffPopupSaveHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupSave>
		(
			[this](const client::event::OffPopupSave& event)
			{
				mIsActive = true;
				if (event.isSave)
				{
					saveSettingData();
				}
				spawnUIObject(mSettingUIPrefab);
				mHaveToDestroyed = true;
			}
		);
}

void fq::client::GameSettingUI::setToggle(int index)
{
	mToggleOnImage[index]->SetIsRender(0, mIsOn[index]);
}

void fq::client::GameSettingUI::setSelectBoxPosition(float dt)
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

void fq::client::GameSettingUI::processInput(float dt)
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
			else
			{
				mIsOn[mSelectButtonID] = !mIsOn[mSelectButtonID];
				setToggle(mSelectButtonID);
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

void fq::client::GameSettingUI::initGameSetting()
{
	mIsOn[0] = true;
	setToggle(0);
	mIsOn[1] = true;
	setToggle(1);
	mIsOn[2] = false;
	setToggle(2);
}

bool fq::client::GameSettingUI::isChangeSettingData()
{
	for (int i = 0; i < 3; i++)
	{
		if (mSaveISOns[i] != mIsOn[i])
		{
			return true;
		}
	}
	return false;
}

void fq::client::GameSettingUI::saveSettingData()
{
	SettingVariable::IsVibe = mIsOn[0];
	SettingVariable::IsUsedAimAssist = mIsOn[1];
	SettingVariable::IsAllowOtherPlayerAttack = mIsOn[2];

	mSaveISOns = mIsOn;
}

std::string fq::client::GameSettingUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

void fq::client::GameSettingUI::spawnUIObject(fq::game_module::PrefabResource prefab)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
}

