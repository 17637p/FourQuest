#include "PauseUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/TextUI.h"

#include "ClientEvent.h"

#include <boost/locale.hpp>

fq::client::PauseUI::PauseUI()
	:mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mUIAnimSpeed(1000),
	mScreenManager(nullptr),
	mIsActive(true),
	mRepauseUIPrefab(),
	mOffPopupRepauseHandler(),
	mSettingUIPrefab(),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mExplanationTexts(),
	mExplanationTextUI()
{
}

fq::client::PauseUI::PauseUI(const PauseUI& other)
	:mSelectButtonID(other.mSelectButtonID),
	mSelectBackground(other.mSelectBackground),
	mButtons(other.mButtons),
	mUIAnimSpeed(other.mUIAnimSpeed),
	mScreenManager(other.mScreenManager),
	mIsActive(other.mIsActive),
	mRepauseUIPrefab(other.mRepauseUIPrefab),
	mSettingUIPrefab(other.mSettingUIPrefab),
	mStickDelay(other.mStickDelay),
	mCurStickDelay(other.mCurStickDelay)
{
}

fq::client::PauseUI& fq::client::PauseUI::operator=(const PauseUI& other)
{
	mSelectButtonID = other.mSelectButtonID;
	mSelectBackground = other.mSelectBackground;
	mButtons = other.mButtons;
	mUIAnimSpeed = other.mUIAnimSpeed;
	mScreenManager = other.mScreenManager;
	mIsActive = other.mIsActive;
	mRepauseUIPrefab = other.mRepauseUIPrefab;
	mSettingUIPrefab = other.mSettingUIPrefab;
	mStickDelay = other.mStickDelay;
	mCurStickDelay = other.mCurStickDelay;

	return *this;
}

fq::client::PauseUI::~PauseUI()
{
}

void fq::client::PauseUI::OnStart()
{
	// 자식 버튼 가져오기
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	auto children = myTransform->GetChildren();

	mButtons.push_back(children[0]->GetGameObject());
	mButtons.push_back(children[1]->GetGameObject());
	mButtons.push_back(children[2]->GetGameObject());
	mButtons.push_back(children[3]->GetGameObject());

	// 선택 UI 초기화 
	mSelectButtonID = 0;
	mSelectBackground = children[4]->GetGameObject();

	// Manager 등록
	mScreenManager = GetScene()->GetScreenManager();
	mTimeManager = GetScene()->GetTimeManager();

	// EventHandler 등록
	EventProcessOffPopupRepause();

	// Text 넣기
	mExplanationTextUI = GetGameObject()->GetChildren()[6]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"게임으로 돌아갑니다."));
	mExplanationTexts.push_back(wstringToString(L"비디오, 오디오 등의 플레이 환경을 설정합니다."));
	mExplanationTexts.push_back(wstringToString(L"현재 진행중인 미션을 종료하고, 준비 단계로 돌아갑니다."));
	mExplanationTexts.push_back(wstringToString(L"4Quest를 종료하고 윈도우로 돌아갑니다."));
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	SetScaleScreen();
	SetSelectBoxPosition(GetScene()->GetTimeManager()->GetDeltaTime());

	if (mIsActive)
	{
		ProcessInput();
		mCurStickDelay += mTimeManager->GetDeltaTime();
	}
}

std::shared_ptr<fq::game_module::Component> fq::client::PauseUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePauseUI = std::dynamic_pointer_cast<PauseUI>(clone);

	if (clonePauseUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		clonePauseUI = game_module::ObjectPool::GetInstance()->Assign<PauseUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*clonePauseUI = *this;
	}

	return clonePauseUI;
}

void fq::client::PauseUI::SetScaleScreen()
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

void fq::client::PauseUI::SetSelectBoxPosition(float dt)
{
	// 선택UI 위치로 SelectBox 옮기기 
	game_module::Transform* selectTransform = mButtons[mSelectButtonID]->GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 selectPosition = selectTransform->GetLocalPosition();

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

	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(curPosition);
}

void fq::client::PauseUI::ClickButton()
{
	switch (mSelectButtonID)
	{
		case 0:
			// 계속 하기
			GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
			GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 1:
			// 설정
			SpawnUIObject(mSettingUIPrefab);
			GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 2:
			// 미션 중단 - repause 소환, 이벤트 받으면 isActive on
			mIsActive = false;
			SpawnUIObject(mRepauseUIPrefab);
			break;
		case 3:
			// 게임 종료
			PostQuitMessage(0);
			break;
		default:
			break;
	}
}

void fq::client::PauseUI::SpawnUIObject(fq::game_module::PrefabResource prefab)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
}

void fq::client::PauseUI::EventProcessOffPopupRepause()
{
	mOffPopupRepauseHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupRepause>
		(
			[this]() 
			{
				mIsActive = true;
			}
	);
}

void fq::client::PauseUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupRepauseHandler);
}

void fq::client::PauseUI::ProcessInput()
{
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
			if (mSelectButtonID < 3)
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

	// B Button
	for (int i = 0; i < 4; i++)
	{
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
	// A Button
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap)
		{
			ClickButton();
		}
	}
}

std::string fq::client::PauseUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}
