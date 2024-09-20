#include "Portal.h"

#include "../FQGameModule/GameModule.h"

#include "GameManager.h"
#include <spdlog/spdlog.h>

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
{

}

fq::client::Portal::~Portal()
{

}

void fq::client::Portal::ChangeScene()
{
	GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({mNextSceneName, true});
}

void fq::client::Portal::OnTriggerEnter(const fq::game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player || collision.other->GetTag() == game_module::ETag::Soul)
	{
		GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>()->SavePlayerState();
		ChangeScene();
	}
}
