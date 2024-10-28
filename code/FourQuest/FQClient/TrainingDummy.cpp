#include "TrainingDummy.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Animator.h"

#include "Attack.h"

std::shared_ptr<fq::game_module::Component> fq::client::TrainingDummy::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneTrainingDummy = std::dynamic_pointer_cast<TrainingDummy>(clone);

	if (cloneTrainingDummy == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneTrainingDummy = game_module::ObjectPool::GetInstance()->Assign<TrainingDummy>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneTrainingDummy = *this;
	}

	return cloneTrainingDummy;
}

fq::client::TrainingDummy::TrainingDummy()
	:mAnimator (nullptr)
{}

fq::client::TrainingDummy::~TrainingDummy()
{}

void fq::client::TrainingDummy::OnTriggerEnter(const game_module::Collision& collision)
{
	if (collision.other->GetTag() == game_module::ETag::PlayerAttack)
	{
		mAnimator->SetParameterTrigger("Hit");

		auto playerAttack = collision.other->GetComponent<Attack>();

		// 피격 사ㅏ운드 재생
		playerAttack->PlayHitSound();

		// 이펙트 방출 
		fq::event::OnCreateStateEvent stateEvent;
		stateEvent.gameObject = GetGameObject();
		stateEvent.RegisterKeyName = playerAttack->GetAttackEffectEvent();
		if (!stateEvent.RegisterKeyName.empty())
		{
			GetGameObject()->GetScene()->GetEventManager()->FireEvent<fq::event::OnCreateStateEvent>(std::move(stateEvent));
		}
	}
}

void fq::client::TrainingDummy::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
}

