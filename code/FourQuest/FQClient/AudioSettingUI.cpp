#include "AudioSettingUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/ImageUI.h"

#include <boost/locale.hpp>
#include <spdlog/spdlog.h>

std::shared_ptr<fq::game_module::Component> fq::client::AudioSettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAudioSettingUI = std::dynamic_pointer_cast<AudioSettingUI>(clone);

	if (cloneAudioSettingUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAudioSettingUI = game_module::ObjectPool::GetInstance()->Assign<AudioSettingUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAudioSettingUI = *this;
	}

	return cloneAudioSettingUI;
}

fq::client::AudioSettingUI::AudioSettingUI()
	:mScreenManager(nullptr),
	mTimeManager(nullptr),
	mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(1000),
	mExplanationTexts(),
	mExplanationTextUI(nullptr),
	mSoundBarWidth(0),
	mSoundBars(),
	mSoundRatio(),
	mSoundBarIcons(),
	mSoundLeftInputTimes(),
	mSoundRightInputTimes(),
	mSoundPercentTexts()
{
}

fq::client::AudioSettingUI::~AudioSettingUI()
{
}

void fq::client::AudioSettingUI::OnStart()
{
	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = scene->GetScreenManager();
	mTimeManager = scene->GetTimeManager();

	// Button Set
	auto buttons = GetGameObject()->GetChildren()[5]->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		mButtons.push_back(buttons[i]);
		mIsMute.push_back(false);
		mMuteCheckImages.push_back(mButtons[i]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mSoundBars.push_back(mButtons[i]->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mSoundBarIcons.push_back(mButtons[i]->GetChildren()[2]->GetChildren()[1]->GetComponent<game_module::Transform>());
		mSoundPercentTexts.push_back(mButtons[i]->GetChildren()[3]->GetComponent<game_module::TextUI>());
		mSoundLeftInputTimes.push_back(0);
		mSoundRightInputTimes.push_back(0);
		mSoundRatio.push_back(0.7f);
	}

	mSoundBarWidth = mSoundBars[0]->GetUIInfomation(0).Width;
	for (int i = 0; i < 4; i++)
	{
		setSoundBar(i);
	}

	mSelectButtonID = 0;
	mSelectBackground = GetGameObject()->GetChildren()[0];

	// Text 넣기
	mExplanationTextUI = GetGameObject()->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"게임의 모든 사운드의 음량을 조절합니다. 슬라이더로 조작합니다."));
	mExplanationTexts.push_back(wstringToString(L"배경 음악의 음량을 조절합니다. 슬라이더로 조작합니다."));
	mExplanationTexts.push_back(wstringToString(L"효과음의 음량을 조절합니다. 슬라이더로 조작합니다."));
	mExplanationTexts.push_back(wstringToString(L"캐릭터의 대사 및 나레이션의 음량을 조절합니다. 슬라이더로 조작합니다."));
}

void fq::client::AudioSettingUI::OnUpdate(float dt)
{
	float deltaTime = mTimeManager->GetDeltaTime();
	setScaleScreen();
	setSelectBoxPosition(deltaTime);

	mCurStickDelay += deltaTime;
	processInput(deltaTime);
}

void fq::client::AudioSettingUI::setScaleScreen()
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

std::string fq::client::AudioSettingUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

void fq::client::AudioSettingUI::processInput(float dt)
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

		// Stick X 값
		float isLeftStickX = input->GetStickInfomation(i, EPadStick::leftX);

		bool isDpadLeftHold = input->GetPadKeyState(i, EPadKey::DpadLeft) == EKeyState::Hold;
		bool isDpadRightHold = input->GetPadKeyState(i, EPadKey::DpadRight) == EKeyState::Hold;

		if (!isDpadLeftHold)
		{
			mSoundLeftInputTimes[i] = 0;
		}
		else
		{
			mSoundLeftInputTimes[i] += dt;
			if (mSoundLeftInputTimes[i] > 0.5f)
			{
				setSound(mSelectButtonID, false);
				continue;
			}
		}
		if (!isDpadRightHold)
		{
			mSoundRightInputTimes[i] = 0;
		}
		else
		{
			mSoundRightInputTimes[i] += dt;
			if (mSoundRightInputTimes[i] > 0.5f)
			{
				setSound(mSelectButtonID, true);
				continue;
			}
		}

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
			setSound(mSelectButtonID, false);
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
			setSound(mSelectButtonID, true);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		// A Button
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap)
		{
			mIsMute[mSelectButtonID] = !mIsMute[mSelectButtonID];
			setMute(mSelectButtonID, mIsMute[mSelectButtonID]);
		}
		// B Button
		if (input->IsPadKeyState(i, EPadKey::B, EKeyState::Tap))
		{
			//GetScene()->GetEventManager()->FireEvent<event::OffPopupSetting>({});
			//GetScene()->DestroyGameObject(GetGameObject());
		}
		// Y Button
		if (input->IsPadKeyState(i, EPadKey::Y, EKeyState::Tap))
		{
			initAudioSetting();
		}
	}
}

void fq::client::AudioSettingUI::setSelectBoxPosition(float dt)
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

	curPosition.x = 470;
	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(curPosition);
}

void fq::client::AudioSettingUI::setMute(int index, bool isMute)
{
	mMuteCheckImages[index]->SetIsRender(0, isMute);
}

void fq::client::AudioSettingUI::setSoundBar(int index)
{
	auto soundBar = mSoundBars[index];
	auto uiInfo = soundBar->GetUIInfomation(0);
	uiInfo.Width = mSoundBarWidth * mSoundRatio[index];
	uiInfo.XRatio = mSoundRatio[index];
	soundBar->SetUIInfomation(0, uiInfo);

	// SoulIcon 옮기기
	DirectX::SimpleMath::Vector3 soulIconT = mSoundBarIcons[index]->GetLocalPosition();
	mSoundBarIcons[index]->SetLocalPosition({ uiInfo.Width - 25, soulIconT.y, soulIconT.z });

	// Set Soul Percent Text
	int percent = mSoundRatio[index] * 100;
	mSoundPercentTexts[index]->SetText(std::to_string(percent) + "%");
}

void fq::client::AudioSettingUI::setSound(int index, bool isUp)
{
	if (isUp && mSoundRatio[index] < 1)
	{
		mSoundRatio[index] += 0.01f;
	}
	if (!isUp && mSoundRatio[index] > 0)
	{
		mSoundRatio[index] -= 0.01f;
	}

	setSoundBar(mSelectButtonID);
}

void fq::client::AudioSettingUI::initAudioSetting()
{
	for (int i = 0; i < 4; i++)
	{
		mSoundRatio[i] = 0.7f;
		setSoundBar(i);
		setMute(i, false);
	}
}
