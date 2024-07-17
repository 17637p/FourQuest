#define NOMINMAX
#include "KnightArmour.h"

fq::client::KnightArmour::KnightArmour()
	:mDashCoolTime(1.f)
	, mDashElapsedTime(0.f)
	, mDashPower(1.f)
	, mController(nullptr)
	, mAnimator(nullptr)
{}

fq::client::KnightArmour::~KnightArmour()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::KnightArmour::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneArmour = std::dynamic_pointer_cast<KnightArmour>(clone);

	if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneArmour = game_module::ObjectPool::GetInstance()->Assign<KnightArmour>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneArmour = *this;
	}

	return cloneArmour;
}

void fq::client::KnightArmour::EmitSwordAttack()
{

}

void fq::client::KnightArmour::EmitShieldAttack()
{

}

void fq::client::KnightArmour::EmitDashAttack()
{

}

void fq::client::KnightArmour::OnUpdate(float dt)
{
	checkSkillCoolTime(dt);
	checkInput();
}

void fq::client::KnightArmour::checkInput()
{
	auto input = GetScene()->GetInputManager();

	// Dash
	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
		&& mDashElapsedTime == 0.f)
	{
		mAnimator->SetParameterTrigger("OnDash");
		mDashElapsedTime = mDashCoolTime;
	}
}

void fq::client::KnightArmour::OnStart()
{
	mAnimator = GetComponent<game_module::Animator>();
	mController = GetComponent<game_module::CharacterController>();

}

void fq::client::KnightArmour::checkSkillCoolTime(float dt)
{
	mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);

}
