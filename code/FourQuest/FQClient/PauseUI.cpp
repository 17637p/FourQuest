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
	// �ڽ� ��ư ��������
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	auto children = myTransform->GetChildren();

	mButtons.push_back(children[0]->GetGameObject());
	mButtons.push_back(children[1]->GetGameObject());
	mButtons.push_back(children[2]->GetGameObject());
	mButtons.push_back(children[3]->GetGameObject());

	// ���� UI �ʱ�ȭ 
	mSelectButtonID = 0;
}

void fq::client::PauseUI::OnUpdate(float dt)
{
	// ����UI�� �ڽ����� ��ȥ �ְ� ��ġ �ű�� 
	
	// UI ���� (�Ͻ����� On, ���� �ű�� ��)
	// �Ƹ� �ؽ�Ʈ�� �־�� �ҵ�?
}
