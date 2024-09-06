#define NOMINMAX
#include "ArcherArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"
#include "Player.h"
#include "Attack.h"
#include "DamageCalculation.h"
#include "LinearAttack.h"

namespace fq::client
{
	ArcherArmour::ArcherArmour()
		: mAnimator()
		, mController()
		, mTransform()
		, mPlayer()
		, mWeakAttack()
		, mStrongAttack()
		, mDashCoolTime()
		, mDashElapsedTime()
		, mArrowPower()
	{
	}

	ArcherArmour::~ArcherArmour()
	{
	}

	void ArcherArmour::EmitWeakAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mWeakAttack);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::Attack>();
		auto rigidBodyComponent = attackObj->GetComponent<fq::game_module::RigidBody>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		auto foward = mTransform->GetLookAtVector();

		// ���� Ʈ������ ����
		DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		pos.y += 1.f;
		attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

		// ���� ����
		AttackInfo attackInfo{};
		attackInfo.damage = dc::GetShieldDamage(mPlayer->GetAttackPower());
		attackInfo.attacker = GetGameObject();
		attackInfo.remainingAttackCount = 1;
		attackInfo.type = EKnockBackType::None;
		attackInfo.attackDirection = foward;
		attackInfo.attackPosition = mTransform->GetWorldPosition();
		attackInfo.hitSound = "A_WeakAttack_Hit";
		attackComponent->Set(attackInfo);

		rigidBodyComponent->SetLinearVelocity(foward * mArrowPower);

		// ShieldAttack �Ҹ�
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_WeakAttack", false , 0 });

		GetScene()->AddGameObject(attackObj);
	}

	void ArcherArmour::EmitStrongAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttack);
		auto& attackObj = *(instance.begin());

		// ���� ����
		AttackInfo attackInfo;
		auto attackComponent = attackObj->GetComponent<client::Attack>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();

		attackInfo.attacker = GetGameObject();
		float attackPower = mPlayer->GetAttackPower();
		attackInfo.damage = dc::GetMagicBallDamage(attackPower);
		attackInfo.bIsInfinite = false;
		attackInfo.remainingAttackCount = 1;
		attackInfo.hitSound = "A_StrongAttack_Hit";
		attackComponent->Set(attackInfo);

		// ���� ��ġ ����
		DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
		pos.y += 1.f;
		attackT->SetLocalPosition(pos);

		// ���� ���� ����
		auto linearAttack = attackObj->GetComponent<LinearAttack>();

		auto direction = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Forward();
		direction.Normalize();
		linearAttack->SetMoveSpeed(mArrowPower);
		linearAttack->SetMoveDirection(direction);

		// MagicBall Attack ����  
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_StrongAttack", false , 0 });

		GetScene()->AddGameObject(attackObj);
	}

	void ArcherArmour::EmitDash()
	{

	}

	void ArcherArmour::OnStart()
	{
		mController = GetComponent<game_module::CharacterController>();
		mAnimator = GetComponent<game_module::Animator>();
		mTransform = GetComponent<game_module::Transform>();
		mPlayer = GetComponent<Player>();
	}

	void ArcherArmour::OnUpdate(float dt)
	{
		checkSkillCoolTime(dt);
		checkInput();
	}

	void ArcherArmour::checkSkillCoolTime(float dt)
	{
		mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
	}

	void ArcherArmour::checkInput()
	{
		using namespace DirectX::SimpleMath;

		auto input = GetScene()->GetInputManager();

		// Dash
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
			&& mDashElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnDash");
			mDashElapsedTime = mDashCoolTime;
		}

		// Shield 
		DirectX::SimpleMath::Vector3 rightInput{};
		rightInput.x = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
		rightInput.z = input->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

		// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		if (rightInput.LengthSquared() >= rotationOffsetSq)
		{
			rightInput.Normalize();

			if (rightInput == Vector3::Backward)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(rightInput, { 0.f,-1.f,0.f }));
			}
			else
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(rightInput, { 0.f,1.f,0.f }));
			}
		}
	}

	std::shared_ptr<fq::game_module::Component> ArcherArmour::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneArmour = std::dynamic_pointer_cast<ArcherArmour>(clone);

		if (cloneArmour == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArcherArmour>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}
}
