#include "Portal.h"

#include "../FQGameModule/GameModule.h"

std::shared_ptr<fq::game_module::Component> fq::client::Portal::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto clonePortal = std::dynamic_pointer_cast<Portal>(clone);

	if (clonePortal == nullptr) // 새로 생성해서 복사본을 준다
	{
		clonePortal = game_module::ObjectPool::GetInstance()->Assign<Portal>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
}

void fq::client::Portal::OnTriggerStay(const fq::game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::Player)
	{
		ChangeScene();
	}
}

void fq::client::Portal::OnTriggerExit(const fq::game_module::Collision& collision)
{}
