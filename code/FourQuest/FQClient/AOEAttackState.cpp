#include "AOEAttackState.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "MagicArmour.h"

fq::client::AOEAttackState::AOEAttackState()
	:mAttackWarningUI(nullptr)
	, mMoveRange(5.f)
	, mCenterPosition{}
{}

fq::client::AOEAttackState::~AOEAttackState()
{
}

std::shared_ptr<fq::game_module::IStateBehaviour> fq::client::AOEAttackState::Clone()
{
	return fq::game_module::ObjectPool::GetInstance()->Assign<AOEAttackState>(*this);
}

void fq::client::AOEAttackState::OnStateEnter(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	// AttackUI 생성 
	auto magicArmour = animator.GetComponent<MagicArmour>();
	auto scene = animator.GetScene();
	auto instance = scene->GetPrefabManager()
		->InstantiatePrefabResoure(magicArmour->GetAttackWarningUI());

	auto& attackUI = *(instance.begin());
	mAttackWarningUI = attackUI.get();

	// UI 위치설정
	auto UIT = mAttackWarningUI->GetComponent<game_module::Transform>();
	auto playerT = animator.GetComponent<game_module::Transform>();

	mCenterPosition = playerT->GetWorldPosition();
	UIT->SetWorldPosition(playerT->GetWorldPosition());
	scene->AddGameObject(attackUI);

	// Controller ID 
	mControllerID = animator.GetComponent<game_module::CharacterController>()->GetControllerID();

	mMoveRange = magicArmour->GetAOEMoveRange();

	// 시작 사운드 재생
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_Explosion_Start", false , fq::sound::EChannel::SE});
}

void fq::client::AOEAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	bool pushA = animator.GetController().GetParameter("PushA").cast<bool>();

	// UI 삭제
	assert(mAttackWarningUI);
	animator.GetScene()->DestroyGameObject(mAttackWarningUI);

	// Hit한 경우
	if (pushA)
	{
		mAttackWarningUI = nullptr;
		return;
	}

	auto magicArmour = animator.GetComponent<MagicArmour>();
	auto attackPoint = mAttackWarningUI->GetComponent<game_module::Transform>()->GetWorldPosition();
	magicArmour->EmitAOE(attackPoint);
	mAttackWarningUI = nullptr;
}

void fq::client::AOEAttackState::OnStateUpdate(game_module::Animator& animator, game_module::AnimationStateNode& state, float dt)
{
	// UI 이동 처리
	auto UIT = mAttackWarningUI->GetComponent<game_module::Transform>();
	auto input = animator.GetScene()->GetInputManager();

	float x = input->GetStickInfomation(mControllerID, EPadStick::leftX);
	float y = input->GetStickInfomation(mControllerID, EPadStick::leftY);

	auto prevPos = UIT->GetWorldPosition();

	constexpr float moveSpeed = 10.f;

	DirectX::SimpleMath::Vector2 moveDistance{ dt * moveSpeed * x, dt * moveSpeed * y };

	auto currentPos = prevPos;
	currentPos.x += moveDistance.x;
	currentPos.z += moveDistance.y;

	auto moveVec = currentPos - mCenterPosition;
	float distance = moveVec.Length();

	// 범위밖으로 나가는 처리 
	if (mMoveRange < distance)
	{
		moveVec.y = 0.f;
		moveVec.Normalize();
		moveVec *= mMoveRange;

		currentPos = mCenterPosition;
		currentPos.x += moveVec.x;
		currentPos.z += moveVec.z;
	}

	UIT->SetWorldPosition(currentPos);
}
