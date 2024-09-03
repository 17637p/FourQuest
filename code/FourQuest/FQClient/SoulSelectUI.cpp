#include "SoulSelectUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::SoulSelectUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto soulSelectUI = std::dynamic_pointer_cast<SoulSelectUI>(clone);

	if (soulSelectUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		soulSelectUI = game_module::ObjectPool::GetInstance()->Assign<SoulSelectUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*soulSelectUI = *this;
	}

	return soulSelectUI;
}

void fq::client::SoulSelectUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// ���� OFF
	for (int i = 0; i < 4; i++)
	{
		SetSpawnButton(i, false);
	}
}

void fq::client::SoulSelectUI::OnUpdate(float dt)
{
	SetScaleScreen();

	// Test ���� ��ư �̵�
	auto input = GetScene()->GetInputManager();
	if (input->GetKeyState(EKey::Z) == EKeyState::Tap)
	{
		SetSpawnButton(0, true);
	}
	if (input->GetKeyState(EKey::X) == EKeyState::Tap)
	{
		SetSpawnButton(1, true);
	}
	if (input->GetKeyState(EKey::C) == EKeyState::Tap)
	{
		SetSpawnButton(2, true);
	}
	if (input->GetKeyState(EKey::V) == EKeyState::Tap)
	{
		SetSpawnButton(3, true);
	}
}

fq::client::SoulSelectUI::SoulSelectUI()
	:mScreenManager(nullptr)
{
}

fq::client::SoulSelectUI::~SoulSelectUI()
{
}

void fq::client::SoulSelectUI::SetSpawnButton(int index, bool isSpawned)
{
	auto children = GetGameObject()->GetChildren()[index]->GetChildren();
	auto soulBox = children[2];
	auto spawnButton = children[3];

	spawnButton->GetComponent<game_module::ImageUI>()->SetIsRender(0, !isSpawned);

	children[1]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
	soulBox->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
	auto soulBoxChildren = soulBox->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		auto textUI = soulBoxChildren[i]->GetComponent<game_module::TextUI>();
		auto textInfo = textUI->GetTextInfo();
		textInfo.IsRender = isSpawned;
		textUI->SetTextInfo(textInfo);
	}
	soulBoxChildren[4]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
}

void fq::client::SoulSelectUI::SetScaleScreen()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale �ڵ� ���� 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}
