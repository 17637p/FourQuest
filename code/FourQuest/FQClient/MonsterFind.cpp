#include "MonsterFind.h"
#include "Monster.h"
#include "PlayerMovement.h"

fq::client::MonsterFind::MonsterFind()
{

}

fq::client::MonsterFind::~MonsterFind()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterFind::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterFind>(*this);
}

void fq::client::MonsterFind::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	Monster* monster = animator.GetComponent<Monster>();

	auto componentView = monster->GetScene()->GetComponentView<PlayerMovement>();

	float distMin = FLT_MAX;

	for (auto& player : componentView)
	{
		float dist = monster->CalculateDistance(player);

		if (distMin > dist)
		{
			distMin = dist;
			monster->SetTarget(&player);
		}
	}

	animator.SetParameterTrigger("OnIdle");
}

void fq::client::MonsterFind::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}

void fq::client::MonsterFind::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}
