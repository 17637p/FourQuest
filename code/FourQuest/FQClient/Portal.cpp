#include "Portal.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/TimeManager.h"

#include "GameManager.h"
#include "ResultUI.h"

std::shared_ptr<fq::game_module::Component> fq::client::Portal::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto clonePortal = std::dynamic_pointer_cast<Portal>(clone);

	if (clonePortal == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		clonePortal = game_module::ObjectPool::GetInstance()->Assign<Portal>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*clonePortal = *this;
	}

	return clonePortal;
}

fq::client::Portal::Portal()
	:mNextSceneName(),
	mResultUIPrefab()
{
}

fq::client::Portal::~Portal()
{
}

void fq::client::Portal::ChangeScene()
{
	GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({mNextSceneName, true});
}

void fq::client::Portal::OnTriggerEnter(const fq::game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player 
		|| collision.other->GetTag() == game_module::ETag::Soul
		|| collision.other->GetTag() == game_module::ETag::Dash
		|| collision.other->GetTag() == game_module::ETag::PlayerMonsterIgnore)
	{
		//GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>()->SavePlayerState();
		ChangeScene();

		// Ÿ�� ������ 0 ���� �ٲٱ� 
		//GetScene()->GetTimeManager()->SetTimeScale(0);
		
		//// ���â ���� 
		//std::shared_ptr<game_module::GameObject> resultUI;
		//
		//auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mResultUIPrefab);
		//resultUI = *(instance.begin());
		//
		//GetScene()->AddGameObject(resultUI);
		//resultUI->GetComponent<ResultUI>()->SetNextScene(mNextSceneName);
	}
}
