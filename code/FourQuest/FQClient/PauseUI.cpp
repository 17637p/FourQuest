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
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale 자동 조정 
	UINT screenWidth = mScreenManager->GetScreenWidth();
	UINT screenHeight = mScreenManager->GetScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}

	// 선택UI의 자식으로 영혼 넣고 위치 옮기기 
	game_module::Transform* selectTransform = mButtons[mSelectButtonID]->GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 selectPosition = selectTransform->GetLocalPosition();

	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(selectPosition);
	
	// UI 조작 (계속하기, 선택 옮기기 등)
	// 아마 텍스트도 넣어야 할듯?
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

