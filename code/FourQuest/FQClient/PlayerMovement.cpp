#include "PlayerMovement.h"

fq::client::PlayerMovement::PlayerMovement()
	:mController(nullptr)
	, mAnimator(nullptr)
{}

fq::client::PlayerMovement::~PlayerMovement()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::PlayerMovement::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<PlayerMovement>(clone);

	if (cloneController == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<PlayerMovement>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::PlayerMovement::OnUpdate(float dt)
{
	constexpr const char* bIsMove = "bIsMove"; 
	auto input = GetScene()->GetInputManager();

	if (input->IsKeyState(EKey::W, EKeyState::Hold)
		|| input->IsKeyState(EKey::S, EKeyState::Hold)
		|| input->IsKeyState(EKey::A, EKeyState::Hold)
		|| input->IsKeyState(EKey::D, EKeyState::Hold))
	{
		mAnimator->SetParameterBoolean(bIsMove, true);
	}
	else
	{
		mAnimator->SetParameterBoolean(bIsMove, false);
	}

	if (input->IsKeyState(EKey::Space, EKeyState::Tap))
	{
		mAnimator->SetParameterTrigger("Jump");
	}

	mAnimator->SetParameterBoolean("OnGround", !mController->IsFalling());
}

void fq::client::PlayerMovement::OnStart()
{
	mAnimator = GetComponent<fq::game_module::Animator>();
	assert(mAnimator);

 	mController =  GetComponent<game_module::Transform>()->GetParentTransform()->GetComponent<fq::game_module::CharacterController>();

}
