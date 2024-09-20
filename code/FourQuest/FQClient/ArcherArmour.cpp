#define NOMINMAX
#include "ArcherArmour.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Animator.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/RigidBody.h"

#include "Player.h"
#include "AimAssist.h"
#include "ArrowAttack.h"
#include "DamageCalculation.h"
#include "LinearAttack.h"

namespace fq::client
{
	ArcherArmour::ArcherArmour()
		: mAnimator(nullptr)
		, mController(nullptr)
		, mTransform()
		, mPlayer(nullptr)
		, mAimAssist(nullptr)
		, mWeakAttack()
		, mStrongAttack()
		, mDashCoolTime()
		, mDashElapsedTime()
		, mArrowPower()
		, mOriginCharacterMaxSpeed()
	{
	}

	ArcherArmour::~ArcherArmour()
	{
	}

	void ArcherArmour::EmitmWeakAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mWeakAttack);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::ArrowAttack>();
		auto rigidBodyComponent = attackObj->GetComponent<fq::game_module::RigidBody>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		auto foward = mTransform->GetLookAtVector();
		DirectX::SimpleMath::Vector3 position{};

		for (const auto& child : mTransform->GetChildren())
		{
			auto name = child->GetGameObject()->GetName();

			if (name.find("WeaponeSocket") != std::string::npos)
			{
				auto transform = child->GetComponent<game_module::Transform>();
				position = transform->GetWorldPosition();
				break;
			}
		}

		// ���� Ʈ������ ����
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		attackT->GenerateWorld(position, rotation, attackT->GetWorldScale());

		// ���� ����
		ArrowAttackInfo attackInfo{};
		attackInfo.weakDamage = dc::GetArcherWADamage(mPlayer->GetAttackPower());
		attackInfo.strongDamage = dc::GetArcherSADamage(mPlayer->GetAttackPower());
		attackInfo.weakProjectileVelocity = mWeakProjectileVelocity;
		attackInfo.strongProjectileVelocity = mStrongProjectileVelocity;
		attackInfo.attacker = GetGameObject();
		attackInfo.remainingAttackCount = 1;
		attackInfo.attackDirection = foward;
		attackInfo.attackTransform = attackT->GetWorldMatrix();
		attackInfo.bIsStrongAttack = false;
		attackInfo.hitSound = "A_WeakAttack_Hit";
		attackComponent->Set(attackInfo);

		// ����� �Ҹ�
		//GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_WeakAttack", false , 0 });

		GetScene()->AddGameObject(attackObj);
	}

	void ArcherArmour::EmitStrongAttack()
	{
		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttack);
		auto& attackObj = *(instance.begin());

		auto attackComponent = attackObj->GetComponent<client::ArrowAttack>();
		auto linearComponent = attackObj->GetComponent<LinearAttack>();
		auto rigidBodyComponent = attackObj->GetComponent<fq::game_module::RigidBody>();
		auto attackT = attackObj->GetComponent<game_module::Transform>();
		auto foward = mTransform->GetLookAtVector();
		DirectX::SimpleMath::Vector3 position{};

		for (const auto& child : mTransform->GetChildren())
		{
			auto name = child->GetGameObject()->GetName();

			if (name.find("WeaponeSocket") != std::string::npos)
			{
				auto transform = child->GetComponent<game_module::Transform>();
				position = transform->GetWorldPosition();
				break;
			}
		}

		// ���� Ʈ������ ����
		DirectX::SimpleMath::Quaternion rotation = mTransform->GetWorldRotation();
		attackT->GenerateWorld(position, rotation, attackT->GetWorldScale());

		// ���� ����
		ArrowAttackInfo attackInfo{};
		attackInfo.weakDamage = dc::GetArcherWADamage(mPlayer->GetAttackPower());
		attackInfo.strongDamage = dc::GetArcherSADamage(mPlayer->GetAttackPower());
		attackInfo.weakProjectileVelocity = mWeakProjectileVelocity;
		attackInfo.strongProjectileVelocity = mStrongProjectileVelocity;
		attackInfo.attacker = GetGameObject();
		attackInfo.attackDirection = foward;
		attackInfo.attackTransform = attackT->GetWorldMatrix();
		attackInfo.bIsStrongAttack = true;
		attackInfo.remainingAttackCount = 0b11111111;
		attackInfo.hitSound = "A_StrongAttack_Hit";
		//GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_StrongAttack", false , 0 });

		attackComponent->Set(attackInfo);

		// MagicBall Attack ����  
		GetScene()->AddGameObject(attackObj);
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitChargingEffect()
	{
		//auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mStrongAttackChargingEffect);
		//auto& chargingObj = *(instance.begin());

		//auto attackT = chargingObj->GetComponent<game_module::Transform>();

		//// Ȱ Ʈ������ ��������
		//attackT->SetParent(mTransform->GetChildren()[3]);

		//GetScene()->AddGameObject(chargingObj);

		// ��¡ ���� ���
		//GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "A_Charging", true , 0 });

		//return chargingObj;

		return nullptr;
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitStrongAttackEffect()
	{
		return nullptr;
	}

	std::shared_ptr<game_module::GameObject> ArcherArmour::EmitDash()
	{
		// �뽬 ����Ʈ ����
		auto name = mAnimator->GetController().GetCurrentStateName();

		auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mDashEffect);
		auto& effectObj = *(instance.begin());
		auto effectT = effectObj->GetComponent<game_module::Transform>();
		effectT->SetParent(mTransform);

		GetScene()->AddGameObject(effectObj);

		return effectObj;
	}

	void ArcherArmour::OnStart()
	{
		mController = GetComponent<game_module::CharacterController>();
		mAnimator = GetComponent<game_module::Animator>();
		mTransform = GetComponent<game_module::Transform>();
		mPlayer = GetComponent<Player>();
		for (auto child : GetGameObject()->GetChildren())
		{
			if (child->HasComponent<AimAssist>())
			{
				mAimAssist = child->GetComponent<AimAssist>();
				break;
			}
		}

		mOriginCharacterMaxSpeed = mController->GetMovementInfo().maxSpeed;
	}

	void ArcherArmour::OnUpdate(float dt)
	{
		checkSkillCoolTime(dt);
		checkInput(dt);
	}

	void ArcherArmour::checkSkillCoolTime(float dt)
	{
		mDashElapsedTime = std::max(0.f, mDashElapsedTime - dt);
		mStrongAttackElapsedTime = std::max(0.f, mStrongAttackElapsedTime - dt);
	}

	void ArcherArmour::checkInput(float dt)
	{
		using namespace DirectX::SimpleMath;

		auto input = GetScene()->GetInputManager();
		auto padID = mController->GetControllerID();

		// Dash
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::A, EKeyState::Tap)
			&& mDashElapsedTime == 0.f)
		{
			mAnimator->SetParameterTrigger("OnDash");
			mDashElapsedTime = mDashCoolTime;
		}
		// StrongAttack
		if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::X, EKeyState::Tap)
			&& mStrongAttackElapsedTime == 0.f)
		{
			mStrongAttackElapsedTime = mStrongAttackCoolTime;
			mAnimator->SetParameterTrigger("OnStrongAttack");
		}

		// MultiShot R Stick ����
		DirectX::SimpleMath::Vector2 r;
		r.x = input->GetStickInfomation(padID, EPadStick::rightX);
		r.y = input->GetStickInfomation(padID, EPadStick::rightY);

		if (r.Length() > 0.f)
		{
			mAnimator->SetParameterBoolean("OnFastShot", true);

			// R Stick�� ���۽����� ������ȯ���� �Է��� 0�� �Ǵ�
			// ������ ��ų�� ĵ���Ǵ� ���� �����ϴ� ���Դϴ�
			constexpr float RStickInputCorrectTime = 0.1f;
			mRStickNoInputTime = RStickInputCorrectTime;
		}
		else
		{
			mRStickNoInputTime = std::max(0.f, mRStickNoInputTime - dt);

			if (mRStickNoInputTime == 0.f)
				mAnimator->SetParameterBoolean("OnFastShot", false);
		}
	}

	void ArcherArmour::SetLookAtRStickInput()
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// ��Ʈ�ѷ� �Է�
		input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightX);
		input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::rightY);

		float lengthSq = input.LengthSquared();

		// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// ĳ���� ��Ʈ�ѷ� ȸ�� ó��
		if (lengthSq >= rotationOffsetSq)
		{
			// �ٶ󺸴� ���� ���� 
			input.Normalize();

			if (input == Vector3::Backward)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,-1.f,0.f }));
			}
			else if (input != Vector3::Zero)
			{
				mTransform->SetWorldRotation(Quaternion::LookRotation(input, { 0.f,1.f,0.f }));
			}
		}
	} 

	void ArcherArmour::SetLookAtLStickInput(float dt, float rotationSpeed)
	{
		using namespace DirectX::SimpleMath;

		auto inputMgr = GetScene()->GetInputManager();
		Vector3 input = Vector3::Zero;

		// ��Ʈ�ѷ� �Է�
		input.x = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftX);
		input.z = inputMgr->GetStickInfomation(mController->GetControllerID(), EPadStick::leftY);

		float lengthSq = input.LengthSquared();

		// ��Ʈ�ѷ� ��ƽ�� ������ ���� �ݵ����� ����� �̼��� ���⼳���� �����ϴ� ��
		constexpr float rotationOffsetSq = 0.5f * 0.5f;

		// ĳ���� ��Ʈ�ѷ� ȸ�� ó��
		if (lengthSq >= rotationOffsetSq)
		{
			// �ٶ󺸴� ���� ���� 
			input.Normalize();

			// �Է� �����κ��� Y�� ȸ�� ������ ���
			float targetYaw = atan2f(-input.x, -input.z); // atan2 ������� XZ ��� ���� ���� ���

			// ���� ȸ�� ���� ������ Y�� ȸ���� �����
			Quaternion startRotation = mTransform->GetWorldRotation();
			Vector3 eulerStartRotation = startRotation.ToEuler();
			eulerStartRotation.x = eulerStartRotation.z = 0.0f; // Y�� ȸ���� ����� �ٸ� ���� 0���� ����

			// ��ǥ ȸ�� �� (Y�� ȸ���� ����)
			Quaternion targetRotation = Quaternion::CreateFromYawPitchRoll(targetYaw, 0.0f, 0.0f);

			// ȸ�� ������ ����
			Quaternion newRotation = Quaternion::Slerp(startRotation, targetRotation, rotationSpeed * dt);

			// �� ȸ�� �� ����
			mTransform->SetWorldRotation(newRotation);
		}
	}

	void ArcherArmour::AimToNearMonster()
	{
		if (mAimAssist)
			mAimAssist->SetNearMonsterDirection();
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
