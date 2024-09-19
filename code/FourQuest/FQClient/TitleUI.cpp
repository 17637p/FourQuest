#include "TitleUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/SceneManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/TimeManager.h"
#include "../FQGameModule/PrefabManager.h"

#include "ClientEvent.h"

std::shared_ptr<fq::game_module::Component> fq::client::TitleUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneTitleUI = std::dynamic_pointer_cast<TitleUI>(clone);

	if (cloneTitleUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneTitleUI = game_module::ObjectPool::GetInstance()->Assign<TitleUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneTitleUI = *this;
	}

	return cloneTitleUI;
}

fq::client::TitleUI::TitleUI()
	:mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mScreenManager(nullptr),
	mUIAnimSpeed(1000),
	mIsActive(true),
	mNextSceneName(""),
	mSettingUIPrefab(),
	mStickDelay(0.2f),
	mCurStickDelay(0)
{
}

fq::client::TitleUI::TitleUI(const TitleUI& other)
	:mSelectButtonID(other.mSelectButtonID),
	mSelectBackground(other.mSelectBackground),
	mButtons(other.mButtons),
	mScreenManager(other.mScreenManager),
	mUIAnimSpeed(other.mUIAnimSpeed),
	mIsActive(other.mIsActive),
	mNextSceneName(other.mNextSceneName),
	mSettingUIPrefab(other.mSettingUIPrefab),
	mStickDelay(other.mStickDelay),
	mCurStickDelay(other.mCurStickDelay)
{
}

fq::client::TitleUI& fq::client::TitleUI::operator=(const TitleUI& other)
{
	mSelectButtonID = other.mSelectButtonID;
	mSelectBackground = other.mSelectBackground;
	mButtons = other.mButtons;
	mScreenManager = other.mScreenManager;
	mUIAnimSpeed = other.mUIAnimSpeed;
	mIsActive = other.mIsActive;
	mNextSceneName = other.mNextSceneName;
	mSettingUIPrefab = other.mSettingUIPrefab;
	mStickDelay = other.mStickDelay;
	mCurStickDelay = other.mCurStickDelay;

	return *this;
}

fq::client::TitleUI::~TitleUI()
{
}

void fq::client::TitleUI::OnStart()
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

	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::TitleUI::OnUpdate(float dt)
{
	setScaleScreen();
	setSelectBoxPosition(GetScene()->GetTimeManager()->GetDeltaTime());

	if (mIsActive)
	{
		processInput();
		mCurStickDelay += dt;
	}
}

void fq::client::TitleUI::setSelectBoxPosition(float dt)
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

void fq::client::TitleUI::setScaleScreen()
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

void fq::client::TitleUI::clickButton()
{
	switch (mSelectButtonID)
	{
		case 0:
			// �κ�� �� �̵�
			GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({ mNextSceneName, true });
			break;
		case 1:
			// ����
			mIsActive = false;
			spawnUIObject(mSettingUIPrefab);
			break;
		case 2:
			// ������
			break;
		case 3:
			// ���� ����
			PostQuitMessage(0);
			break;
		default:
			break;
	}
}

void fq::client::TitleUI::eventProcessOffPopupSetting()
{
	mOffPopupSettingHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::OffPopupSetting>
		(
			[this]()
			{
				mIsActive = true;
			}
		);
}

void fq::client::TitleUI::spawnUIObject(fq::game_module::PrefabResource prefab)
{
	std::shared_ptr<game_module::GameObject> newObject;

	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(prefab);
	newObject = *(instance.begin());

	GetScene()->AddGameObject(newObject);
}

void fq::client::TitleUI::processInput()
{
	// Setting Popup�� ���� ���� ���
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
			}
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap)
		{
			clickButton();
		}
	}
}

