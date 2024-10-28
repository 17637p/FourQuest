#include "TrainingDummy.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Animator.h"

#include "Attack.h"

std::shared_ptr<fq::game_module::Component> fq::client::TrainingDummy::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneTrainingDummy = std::dynamic_pointer_cast<TrainingDummy>(clone);

	if (cloneTrainingDummy == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneTrainingDummy = game_module::ObjectPool::GetInstance()->Assign<TrainingDummy>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
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

		// �ǰ� �礿��� ���
		playerAttack->PlayHitSound();

		// ����Ʈ ���� 
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

