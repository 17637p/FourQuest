#include "MonsterDie.h"
#include "Monster.h"

fq::client::MonsterDie::MonsterDie()
{

}

fq::client::MonsterDie::~MonsterDie()
{

}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::MonsterDie::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<MonsterDie>(*this);
}

void fq::client::MonsterDie::OnStateEnter(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{

}

void fq::client::MonsterDie::OnStateUpdate(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	
}

void fq::client::MonsterDie::OnStateExit(fq::game_module::Animator& animator, fq::game_module::AnimationStateNode& state)
{
	Monster* monster = animator.GetComponent<Monster>();
	monster->GetScene()->DestroyGameObject(monster->GetGameObject());
}
