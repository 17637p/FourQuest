#include "TitleUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/SceneManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"

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
	mUIAnimSpeed(1000)
{
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
	SetScaleScreen();
	SetSelectBoxPosition(dt);

	// Test ���� ��ư �̵�
	auto input = GetScene()->GetInputManager();
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

void fq::client::TitleUI::SetSelectBoxPosition(float dt)
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

void fq::client::TitleUI::SetScaleScreen()
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

void fq::client::TitleUI::ClickButton()
{
	switch (mSelectButtonID)
	{
		case 0:
			// �κ�� �� �̵�
			GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({ mNextSceneName, true });
			break;
		case 1:
			// ����
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
