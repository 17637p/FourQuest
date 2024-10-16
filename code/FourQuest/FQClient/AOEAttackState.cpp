#include "AOEAttackState.h"
#include "EffectColorTransmitter.h"

#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/RigidBody.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/Controller.h"
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
	// AttackUI ���� 
	auto magicArmour = animator.GetComponent<MagicArmour>();
	auto scene = animator.GetScene();
	auto instance = scene->GetPrefabManager()
		->InstantiatePrefabResoure(magicArmour->GetAttackWarningUI());

	auto& attackUI = *(instance.begin());
	mAttackWarningUI = attackUI.get();

	// UI ��ġ����
	auto UIT = mAttackWarningUI->GetComponent<game_module::Transform>();
	auto playerT = animator.GetComponent<game_module::Transform>();

	mCenterPosition = playerT->GetWorldPosition();
	UIT->SetWorldPosition(playerT->GetWorldPosition());
	scene->AddGameObject(attackUI);

	// Controller ID 
	mControllerID = animator.GetComponent<game_module::CharacterController>()->GetControllerID();

	// ���� ���� UI ��Ʈ�ѷ��� ����
	auto controller = mAttackWarningUI->GetComponent<fq::game_module::Controller>();

	if (controller != nullptr)
	{
		controller->SetControllerID(mControllerID);
	}

	mMoveRange = magicArmour->GetAOEMoveRange();

	// ���� ���� ���
	animator.GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "M_Explosion_Start", false , fq::sound::EChannel::SE });

	// ����Ʈ ���� ����
	auto player = animator.GetComponent<Player>();
	auto effectColorTransmitter = attackUI->GetComponent<EffectColorTransmitter>();
	if (effectColorTransmitter != nullptr && player != nullptr)
	{
		effectColorTransmitter->SetSoulType(player->GetSoulType());
	}
}

void fq::client::AOEAttackState::OnStateExit(game_module::Animator& animator, game_module::AnimationStateNode& state)
{
	bool pushA = animator.GetController().GetParameter("PushA").cast<bool>();

	// UI ����
	assert(mAttackWarningUI);
	animator.GetScene()->DestroyGameObject(mAttackWarningUI);

	// Hit�� ���
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
}
