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
	auto input = GetScene()->GetInputManager();

	if (input->IsPadKeyState(0, EPadKey::A, EKeyState::Tap))
	{
		mAnimator->SetParameterTrigger("OnHit");
	}
	if (input->IsPadKeyState(0, EPadKey::X, EKeyState::Tap))
	{
		mAnimator->SetParameterTrigger("OnPunch");
	}

	float leftX = input->GetStickInfomation(0, EPadStick::leftX);
	float leftY = input->GetStickInfomation(0, EPadStick::leftY);
	float rightX = input->GetStickInfomation(0, EPadStick::rightX);
	float rightY = input->GetStickInfomation(0, EPadStick::rightY);

	spdlog::trace("left X : {}", leftX);
	spdlog::trace("left Y : {}", leftY);
	spdlog::trace("right X : {}", rightX);
	spdlog::trace("right Y : {}", rightY);

}

void fq::client::PlayerMovement::OnStart()
{
	mAnimator = GetComponent<fq::game_module::Animator>();
	assert(mAnimator);

	mController = GetComponent<fq::game_module::CharacterController>();
}
