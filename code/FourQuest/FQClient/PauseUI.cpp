#include "PauseUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"

fq::client::PauseUI::PauseUI()
	:mSelectButtonID(0)
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
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale �ڵ� ���� 
	UINT screenWidth = mScreenManager->GetScreenWidth();
	UINT screenHeight = mScreenManager->GetScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}

	// ����UI�� �ڽ����� ��ȥ �ְ� ��ġ �ű�� 
	game_module::Transform* selectTransform = mButtons[mSelectButtonID]->GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 selectPosition = selectTransform->GetLocalPosition();

	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(selectPosition);
	
	// UI ���� (����ϱ�, ���� �ű�� ��)
	// �Ƹ� �ؽ�Ʈ�� �־�� �ҵ�?
	auto input = GetScene()->GetInputManager();
	if (input->IsPadKeyState(0, EPadKey::B, EKeyState::Tap)
		|| input->IsPadKeyState(1, EPadKey::B, EKeyState::Tap)
		|| input->IsPadKeyState(2, EPadKey::B, EKeyState::Tap)
		|| input->IsPadKeyState(3, EPadKey::B, EKeyState::Tap)
		|| input->IsKeyState(EKey::O, EKeyState::Tap))
	{
		GetScene()->DestroyGameObject(GetGameObject());
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

