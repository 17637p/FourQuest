#include "PauseUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/PrefabManager.h"

#include "ClientEvent.h"

fq::client::PauseUI::PauseUI()
	:mSelectButtonID(0),
	mSelectBackground(nullptr),
	mButtons(),
	mUIAnimSpeed(1000),
	mScreenManager(nullptr),
	mIsActive(true),
	mRepauseUIPrefab(),
	mOffPopupRepauseHandler(),
	mSettingUIPrefab()
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
	mSettingUIPrefab(other.mSettingUIPrefab)
{
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

	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// EventHandler ���
	EventProcessOffPopupRepause();
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	SetScaleScreen();
	SetSelectBoxPosition(GetScene()->GetTimeManager()->GetDeltaTime());

	if (mIsActive)
	{
		// UI ���� (����ϱ�, ���� �ű�� ��)
		// �Ƹ� �ؽ�Ʈ�� �־�� �ҵ�?
		auto input = GetScene()->GetInputManager();
		if (input->IsPadKeyState(0, EPadKey::B, EKeyState::Tap)
			|| input->IsPadKeyState(1, EPadKey::B, EKeyState::Tap)
			|| input->IsPadKeyState(2, EPadKey::B, EKeyState::Tap)
			|| input->IsPadKeyState(3, EPadKey::B, EKeyState::Tap)
			|| input->IsKeyState(EKey::O, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<event::OffPopupPause>({});
			GetScene()->DestroyGameObject(GetGameObject());
		}

		if (input->GetKeyState(EKey::S) == EKeyState::Tap)
		{
			if (mSelectButtonID < 3)
			{
				mSelectButtonID++;
			}
		}
		if (input->GetKeyState(EKey::W) == EKeyState::Tap)
		{
			if (mSelectButtonID > 0)
			{
				mSelectButtonID--;
			}
		}

		if (input->GetKeyState(EKey::F) == EKeyState::Tap)
		{
			ClickButton();
		}
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

	return *this;
}

void fq::client::PauseUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mOffPopupRepauseHandler);
}
