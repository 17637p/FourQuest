#include "PauseUI.h"
#include "../FQGameModule/Transform.h"

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
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	// 선택UI의 자식으로 영혼 넣고 위치 옮기기 
	
	// UI 조작 (일시정지 On, 선택 옮기기 등)
	// 아마 텍스트도 넣어야 할듯?
}
