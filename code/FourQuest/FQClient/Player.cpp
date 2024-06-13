#include "Player.h"



fq::client::Player::Player()
{

}

fq::client::Player::~Player()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Player::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<Player>(clone);

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<Player>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::Player::OnUpdate(float dt)
{
	mAnimator->SetParameterBoolean("OnMove", mController->OnMove());

	processDash();
}

void fq::client::Player::OnStart()
{
	mAnimator = GetComponent<fq::game_module::Animator>();
	assert(mAnimator);

	mController = GetComponent<fq::game_module::CharacterController>();
}

void fq::client::Player::processDash()
{
	auto input = GetScene()->GetInputManager();

	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap))
	{
		mAnimator->SetParameterTrigger("OnDash");
	}
}

void fq::client::Player::OnCollisionEnter(const game_module::Collision& collision)
{
}

void fq::client::Player::OnCollisionExit(const game_module::Collision& collision)
{
}

void fq::client::Player::OnTriggerExit(const game_module::Collision& collision)
{
}

void fq::client::Player::OnTriggerEnter(const game_module::Collision& collision)
{
}

void fq::client::Player::Attack()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(GetAttackPrefab());
	auto& attack = *(instance.begin());

	auto attackT = attack->GetComponent<game_module::Transform>();
	auto transform = GetComponent<game_module::Transform>();

	auto forward = transform->GetWorldMatrix().Forward();
	forward.Normalize();

	attack->SetTag(game_module::ETag::PlayerAttack);

	attackT->SetLocalRotation(transform->GetWorldRotation());
	attackT->SetLocalPosition(transform->GetWorldPosition() + forward);

	GetScene()->AddGameObject(attack);
}
