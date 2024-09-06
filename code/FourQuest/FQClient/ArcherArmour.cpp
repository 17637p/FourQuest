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

		// 공격 트랜스폼 설정
		DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		pos.y += 1.f;
		attackT->GenerateWorld(pos, rotation, attackT->GetWorldScale());

		// 공격 설정
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

		// ShieldAttack 소리
		GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_WeakAttack", false , 0 });

		GetScene()->AddGameObject(attackObj);
	}

	void ArcherArmour::EmitStrongAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttack);
		auto& attackObj = *(instance.begin());

		// 공격 설정
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

		// 공격 위치 설정
		DirectX::SimpleMath::Vector3 pos = mTransform->GetWorldPosition();
		pos.y += 1.f;
		attackT->SetLocalPosition(pos);

		// 공격 방향 설정
		auto linearAttack = attackObj->GetComponent<LinearAttack>();

		auto direction = DirectX::SimpleMath::Matrix::CreateFromQuaternion(mTransform->GetWorldRotation()).Forward();
		direction.Normalize();
		linearAttack->SetMoveSpeed(mArrowPower);
		linearAttack->SetMoveDirection(direction);

		// MagicBall Attack 사운드  
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

		// 컨트롤러 스틱을 조작하 땔때 반동으로 생기는 미세한 방향설정을 무시하는 값
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

		if (cloneArmour == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneArmour = game_module::ObjectPool::GetInstance()->Assign<ArcherArmour>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneArmour = *this;
		}

		return cloneArmour;
	}
}
