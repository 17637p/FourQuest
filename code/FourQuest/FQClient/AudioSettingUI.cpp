#include "AudioSettingUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/SoundManager.h"
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

std::shared_ptr<fq::game_module::Component> fq::client::AudioSettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAudioSettingUI = std::dynamic_pointer_cast<AudioSettingUI>(clone);

	if (cloneAudioSettingUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAudioSettingUI = game_module::ObjectPool::GetInstance()->Assign<AudioSettingUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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
	mSoundPercentTexts(),
	mIsActive(true),
	mResetMessagePrefab(),
	mSaveMessagePrefab(),
	mSettingUIPrefab(),
	mHaveToDestroyed(false)
{
}

fq::client::AudioSettingUI::AudioSettingUI(const AudioSettingUI& other)
	:mScreenManager(nullptr),
	mTimeManager(nullptr),
	mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(other.mUIAnimSpeed),
	mExplanationTexts(),
	mExplanationTextUI(nullptr),
	mSoundBarWidth(0),
	mSoundBars(),
	mSoundRatio(),
	mSoundBarIcons(),
	mSoundLeftInputTimes(),
	mSoundRightInputTimes(),
	mSoundPercentTexts(),
	mIsActive(true),
	mResetMessagePrefab(other.mResetMessagePrefab),
	mSaveMessagePrefab(other.mSaveMessagePrefab),
	mSettingUIPrefab(other.mSettingUIPrefab),
	mHaveToDestroyed(false)
{
}

fq::client::AudioSettingUI& fq::client::AudioSettingUI::operator=(const AudioSettingUI& other)
{
	mSelectButtonID = other.mSelectButtonID;
	mStickDelay = other.mStickDelay;
	mUIAnimSpeed = other.mUIAnimSpeed;
	mIsActive = other.mIsActive;
	mResetMessagePrefab = other.mResetMessagePrefab;
	mSaveMessagePrefab = other.mSaveMessagePrefab;
	mSettingUIPrefab = other.mSettingUIPrefab;
	mHaveToDestroyed = false;

	return *this;
}

fq::client::AudioSettingUI::~AudioSettingUI()
{
}

void fq::client::AudioSettingUI::OnStart()
{
	mButtons.clear();
	mExplanationTexts.clear();
	mIsMutes.clear();
	mSoundRatio.clear();
	mSoundLeftInputTimes.clear();
	mSoundRightInputTimes.clear();
	mSoundBars.clear();
	mSoundBarIcons.clear();
	mMuteCheckImages.clear();
	mSoundPercentTexts.clear();
	mSaveISMutes.clear();
	mSaveSoundRatio.clear();

	mIsActive = true;
	mHaveToDestroyed = false;

	// Event ���
	eventProcessOffPopupReset();
	eventProcessOffPopupSave();

	// Manager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = scene->GetScreenManager();
	mTimeManager = scene->GetTimeManager();
	mSoundManager = scene->GetSoundManager();

	// Button Set
	auto buttons = GetGameObject()->GetChildren()[5]->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		mButtons.push_back(buttons[i]);
		mMuteCheckImages.push_back(mButtons[i]->GetChildren()[1]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mSoundBars.push_back(mButtons[i]->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::ImageUI>());
		mSoundBarIcons.push_back(mButtons[i]->GetChildren()[2]->GetChildren()[1]->GetComponent<game_module::Transform>());
		mSoundPercentTexts.push_back(mButtons[i]->GetChildren()[3]->GetComponent<game_module::TextUI>());
		mSoundLeftInputTimes.push_back(0);
		mSoundRightInputTimes.push_back(0);
	}

	mSoundRatio.push_back(SettingVariable::MasterVolume);
	mSoundRatio.push_back(SettingVariable::BGMVolume);
	mSoundRatio.push_back(SettingVariable::SFXVolume);
	mSoundRatio.push_back(SettingVariable::VoiceVolume);

	mIsMutes.push_back(SettingVariable::MuteMasterVolume);
	mIsMutes.push_back(SettingVariable::MuteBGMVolume);
	mIsMutes.push_back(SettingVariable::MuteSFXVolume);
	mIsMutes.push_back(SettingVariable::MuteVoiceVolume);

	mSaveISMutes = mIsMutes;
	mSaveSoundRatio = mSoundRatio;

	mSoundBarWidth = mSoundBars[0]->GetUIInfomation(0).Width;
	for (int i = 0; i < 4; i++)
	{
		setSoundBar(i);
		setMute(i);
	}

	mSelectButtonID = 0;
	mSelectBackground = GetGameObject()->GetChildren()[0];

	mSelectButtonBackground = GetGameObject()->GetChildren()[3]->GetChildren()[0]->GetComponent<game_module::ImageUI>();
	mSelectButtonSoulIcon = GetGameObject()->GetChildren()[3]->GetChildren()[1]->GetComponent<game_module::SpriteAnimationUI>();

	// Text �ֱ�
	mExplanationTextUI = GetGameObject()->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"������ ��� ������ ������ �����մϴ�. �����̴��� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"��� ������ ������ �����մϴ�. �����̴��� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"ȿ������ ������ �����մϴ�. �����̴��� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"ĳ������ ��� �� �����̼��� ������ �����մϴ�. �����̴��� �����մϴ�."));
}

void fq::client::AudioSettingUI::OnUpdate(float dt)
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

void fq::client::AudioSettingUI::setScaleScreen()
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

std::string fq::client::AudioSettingUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

void fq::client::AudioSettingUI::processInput(float dt)
{
	mCurStickDelay += dt;

	// UI ���� (����ϱ�, ���� �ű�� ��)
	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		// Stick Y ��
		float isLeftStickY = input->GetStickInfomation(i, EPadStick::leftY);

		// �Ʒ���
		bool isDpadDownTap = input->GetPadKeyState(i, EPadKey::DpadDown) == EKeyState::Tap;
		// Stick ó��
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
			if (mSelectButtonID == 3)
			{
				mSelectButtonID++;
			}
			if (mSelectButtonID < 3)
			{
				mSelectButtonID++;
				mExplanationTextUI->SetText(mExplanationTexts[mSelectButtonID]);
			}
		}

		// ����
		bool isDpadUpTap = input->GetPadKeyState(i, EPadKey::DpadUp) == EKeyState::Tap;
		// Stick ó��
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

		// Stick X ��
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

		// �·�
		bool isDpadLeftTap = input->GetPadKeyState(i, EPadKey::DpadLeft) == EKeyState::Tap;
		// Stick ó��
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

		// ���
		bool isDpadRightTap = input->GetPadKeyState(i, EPadKey::DpadRight) == EKeyState::Tap;
		// Stick ó��
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
			if (mSelectButtonID == 4)
			{
				saveSettingData();
			}
			else
			{
				mIsMutes[mSelectButtonID] = !mIsMutes[mSelectButtonID];
				setMute(mSelectButtonID);
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

void fq::client::AudioSettingUI::setSelectBoxPosition(float dt)
{
	bool isSelectAcceptButton = false;
	if (mSelectButtonID == 4)
	{
		isSelectAcceptButton = true;
	}

	mSelectButtonBackground->SetIsRender(0, isSelectAcceptButton);
	mSelectButtonSoulIcon->SetIsRender(isSelectAcceptButton);

	mExplanationTextUI->SetIsRender(!isSelectAcceptButton);
	mSelectBackground->GetComponent<game_module::ImageUI>()->SetIsRender(0, !isSelectAcceptButton);
	mSelectBackground->GetChildren()[0]->GetComponent<game_module::SpriteAnimationUI>()->SetIsRender(!isSelectAcceptButton);

	if (mSelectButtonID == 4)
	{
		return;
	}

	// ����UI ��ġ�� SelectBox �ű�� 
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

void fq::client::AudioSettingUI::setMute(int index)
{
	mMuteCheckImages[index]->SetIsRender(0, mIsMutes[index]);
}

void fq::client::AudioSettingUI::setSoundBar(int index)
{
	auto soundBar = mSoundBars[index];
	auto uiInfo = soundBar->GetUIInfomation(0);
	uiInfo.Width = mSoundBarWidth * mSoundRatio[index];
	uiInfo.XRatio = mSoundRatio[index];
	soundBar->SetUIInfomation(0, uiInfo);

	// SoulIcon �ű��
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
		mIsMutes[i] = false;
		setMute(i);
	}
}

void fq::client::AudioSettingUI::spawnUIObject(fq::game_module::PrefabResource prefab)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
}

void fq::client::AudioSettingUI::eventProcessOffPopupReset()
{
	mOffPopupResetHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupReset>
		(
			[this](const client::event::OffPopupReset& event)
			{
				mIsActive = true;
				if (event.isReset)
				{
					initAudioSetting();
				}
			}
		);
}

void fq::client::AudioSettingUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupResetHandler);
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupSaveHandler);
}

bool fq::client::AudioSettingUI::isChangeSettingData()
{
	for (int i = 0; i < 4; i++)
	{
		if (mSaveISMutes[i] != mIsMutes[i])
		{
			return true;
		}
		if (mSaveSoundRatio[i] != mSoundRatio[i])
		{
			return true;
		}
	}
	return false;
}

void fq::client::AudioSettingUI::eventProcessOffPopupSave()
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

void fq::client::AudioSettingUI::saveSettingData()
{
	mSoundManager->SetChannelVoulme(sound::BGM, mSoundRatio[1]);
	mSoundManager->SetChannelVoulme(sound::SE, mSoundRatio[2]);
	//mSoundManager->SetChannelVoulme(sound::BGM, mSoundRatio[3]);
	
	SettingVariable::MasterVolume = mSoundRatio[0];
	SettingVariable::BGMVolume = mSoundRatio[1];
	SettingVariable::SFXVolume = mSoundRatio[2];
	SettingVariable::VoiceVolume = mSoundRatio[3];

	SettingVariable::MuteMasterVolume = mIsMutes[0];
	SettingVariable::MuteBGMVolume = mIsMutes[1];
	SettingVariable::MuteSFXVolume = mIsMutes[2];
	SettingVariable::MuteVoiceVolume = mIsMutes[3];

	if (mIsMutes[0])
	{
		mSoundManager->SetChannelVoulme(sound::BGM, 0);
		mSoundManager->SetChannelVoulme(sound::SE, 0);
	}
	if (mIsMutes[1])
	{
		mSoundManager->SetChannelVoulme(sound::BGM, 0);
	}
	if (mIsMutes[2])
	{
		mSoundManager->SetChannelVoulme(sound::SE, 0);
	}
	if (mIsMutes[3])
	{
		//mSoundManager->SetChannelVoulme(sound::voice, 0);
	}

	mSaveSoundRatio = mSoundRatio;
	mSaveISMutes = mIsMutes;
}