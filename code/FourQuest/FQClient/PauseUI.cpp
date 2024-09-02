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

	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// EventHandler 등록
	EventProcessOffPopupRepause();
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	SetScaleScreen();
	SetSelectBoxPosition(GetScene()->GetTimeManager()->GetDeltaTime());

	if (mIsActive)
	{
		// UI 조작 (계속하기, 선택 옮기기 등)
		// 아마 텍스트도 넣어야 할듯?
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
