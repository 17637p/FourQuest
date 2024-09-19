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
	// �ڽ� ��ư ��������
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	auto children = myTransform->GetChildren();

	mButtons.push_back(children[0]->GetGameObject());
	mButtons.push_back(children[1]->GetGameObject());
	mButtons.push_back(children[2]->GetGameObject());
	mButtons.push_back(children[3]->GetGameObject());

	// ���� UI �ʱ�ȭ 
	mSelectButtonID = 0;
	mSelectBackground = children[4]->GetGameObject();

	// Manager ���
	mScreenManager = GetScene()->GetScreenManager();
	mTimeManager = GetScene()->GetTimeManager();

	// EventHandler ���
	EventProcessOffPopupRepause();

	// Text �ֱ�
	mExplanationTextUI = GetGameObject()->GetChildren()[6]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"�������� ���ư��ϴ�."));
	mExplanationTexts.push_back(wstringToString(L"����, ����� ���� �÷��� ȯ���� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"���� �������� �̼��� �����ϰ�, �غ� �ܰ�� ���ư��ϴ�."));
	mExplanationTexts.push_back(wstringToString(L"4Quest�� �����ϰ� ������� ���ư��ϴ�."));
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

	if (clonePauseUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		clonePauseUI = game_module::ObjectPool::GetInstance()->Assign<PauseUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*clonePauseUI = *this;
	}

	return clonePauseUI;
}

void fq::client::PauseUI::SetScaleScreen()
{
	// Scale �ڵ� ���� 
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

	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(curPosition);
}

void fq::client::PauseUI::ClickButton()
{
	switch (mSelectButtonID)
	{
		case 0:
			// ��� �ϱ�
			GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
			GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 1:
			// ����
			SpawnUIObject(mSettingUIPrefab);
			GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 2:
			// �̼� �ߴ� - repause ��ȯ, �̺�Ʈ ������ isActive on
			mIsActive = false;
			SpawnUIObject(mRepauseUIPrefab);
			break;
		case 3:
			// ���� ����
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
