#include "SettingUI.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/TextUI.h"

#include "ClientEvent.h"

#include <boost/locale.hpp>

fq::client::SettingUI::SettingUI()
	:mScreenManager(nullptr),
	mTimeManager(nullptr),
	mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(1000),
	mExplanationTexts(),
	mExplanationTextUI(nullptr)
{

}

fq::client::SettingUI::~SettingUI()
{
}

void fq::client::SettingUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = scene->GetScreenManager();
	mTimeManager = scene->GetTimeManager();

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

	// Text �ֱ�
	mExplanationTextUI = GetGameObject()->GetChildren()[6]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"���� ȯ���� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"�ػ� �� ȭ�� ȯ���� �����մϴ�."));
	mExplanationTexts.push_back(wstringToString(L"����� ������ ������ �� �ֽ��ϴ�."));
	mExplanationTexts.push_back(wstringToString(L"�������� ���ư��ϴ�."));
}

void fq::client::SettingUI::OnUpdate(float dt)
{
	setScaleScreen();
	setSelectBoxPosition(mTimeManager->GetDeltaTime());

	mCurStickDelay += mTimeManager->GetDeltaTime();
	processInput();
}

std::shared_ptr<fq::game_module::Component> fq::client::SettingUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSettingUI = std::dynamic_pointer_cast<SettingUI>(clone);

	if (cloneSettingUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneSettingUI = game_module::ObjectPool::GetInstance()->Assign<SettingUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneSettingUI = *this;
	}

	return cloneSettingUI;
}

void fq::client::SettingUI::setScaleScreen()
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

void fq::client::SettingUI::setSelectBoxPosition(float dt)
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

void fq::client::SettingUI::processInput()
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
			GetScene()->GetEventManager()->FireEvent<event::OffPopupSetting>({});
			GetScene()->DestroyGameObject(GetGameObject());
		}
	}
	// A Button
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap)
		{
			clickButton();
		}
	}
}

std::string fq::client::SettingUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

void fq::client::SettingUI::clickButton()
{
	switch (mSelectButtonID)
	{
		case 0:
			// ����
			//GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
			//GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 1:
			// ����
			//spawnUIObject(mSettingUIPrefab);
			//GetScene()->DestroyGameObject(GetGameObject());
			break;
		case 2:
			// �����
			//mIsActive = false;
			//spawnUIObject(mRepauseUIPrefab);
			break;
		case 3:
			// ���ư���
			//GetScene()->DestroyGameObject(GetGameObject());
			break;
		default:
			break;
	}
}

//GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
