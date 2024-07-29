#define NOMINMAX
#include "Player.h"


#include "Attack.h"
#include "CameraMoving.h"
#include "HpBar.h"
#include "Soul.h"
#include "ClientEvent.h"

fq::client::Player::Player()
	:mAttackPower(1.f)
	, mAttackPrafab{}
	, mController(nullptr)
	, mHp(0.f)
	, mMaxHp(0.f)
	, mInvincibleTime(1.f)
	, mInvincibleElapsedTime(0.f)
	, mAnimator(nullptr)
	, mSoulStack(0.f)
	, mFeverTime(0.f)
	, mSoulType(ESoulType::Sword)
	, mArmourType(EArmourType::Knight)
	, mAttackPositionOffset{}
	, mbOnShieldBlock(false)
	, mTransform(nullptr)
	, mAttackSpeed(1.f)
{}

fq::client::Player::~Player()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::Player::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneController = std::dynamic_pointer_cast<Player>(clone);

	if (cloneController == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneController = game_module::ObjectPool::GetInstance()->Assign<Player>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneController = *this;
	}

	return cloneController;
}

void fq::client::Player::OnUpdate(float dt)
{
	mAnimator->SetParameterBoolean("OnMove", mController->OnMove());

	processInput();
	processFeverTime(dt);
	processCoolTime(dt);
}

void fq::client::Player::OnStart()
{
	mMaxHp = mHp;

	mAnimator = GetComponent<fq::game_module::Animator>();
	mController = GetComponent<fq::game_module::CharacterController>();
	mTransform = GetComponent<fq::game_module::Transform>();

	// ���� ��������  
	mSoundClip = GetScene()->GetObjectByName("SoundManager")->GetComponent<game_module::SoundClip>();

	// Player���
	GetScene()->GetEventManager()->FireEvent<client::event::RegisterPlayer>(
		{ GetGameObject(), EPlayerType::LivingArmour });

	// ī�޶� �÷��̾� ��� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.AddPlayerTransform(GetComponent<game_module::Transform>());
		});

	// ��ȥ Ÿ�� ����
	mAnimator->SetParameterInt("SoulType", static_cast<int>(mSoulType));

	// TODO : ���� ���� ����
}

void fq::client::Player::processInput()
{
	auto input = GetScene()->GetInputManager();


	if (input->IsPadKeyState(mController->GetControllerID(), EPadKey::B, EKeyState::Tap))
	{
		SummonSoul();
	}
}


void fq::client::Player::OnDestroy()
{
	// ī�޶� �÷��̾� ���� 
	GetScene()->ViewComponents<CameraMoving>([this](game_module::GameObject& object, CameraMoving& camera)
		{
			camera.DeletePlayerTransform(GetComponent<game_module::Transform>());
		});
}

void fq::client::Player::OnTriggerEnter(const game_module::Collision& collision)
{
	bool isHitAble = mInvincibleElapsedTime == 0.f;

	// �÷��̾� �ǰ�
	if (isHitAble && collision.other->GetTag() == game_module::ETag::MonsterAttack)
	{
		auto monsterAtk = collision.other->GetComponent<client::Attack>();
		if (monsterAtk->ProcessAttack())
		{
			// �÷��̾� ���� ���� ó�� 
			if (mbOnShieldBlock)
			{
				auto attackDir = monsterAtk->GetAttackDirection();
				auto lookAtDir = mTransform->GetLookAtVector();

				attackDir.Normalize();
				lookAtDir.Normalize();

				float dotProduct = lookAtDir.Dot(attackDir);
				float radian = std::acos(dotProduct);

				if (radian >= DirectX::XM_PIDIV2)
				{
					// TODO :: Shield Block �Ҹ� , ����Ʈ  �߰�
					mSoundClip->Play("ShieldBlock", false, 0);

					return;
				}
			}

			mAnimator->SetParameterTrigger("OnHit");
			float attackPower = monsterAtk->GetAttackPower();
			mHp -= attackPower;

			mInvincibleElapsedTime = mInvincibleTime;

			// UI ����
			GetComponent<HpBar>()->DecreaseHp(attackPower / mMaxHp);

			// �÷��̾� ���ó�� 
			if (mHp <= 0.f)
			{
				SummonSoul();
			}
		}
	}
}

void fq::client::Player::SummonSoul()
{
	auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
	auto& soul = *(instance.begin());

	// ��Ʈ�ѷ� ����
	soul->GetComponent<game_module::CharacterController>()
		->SetControllerID(mController->GetControllerID());

	// �ҿ� Ÿ�� ����
	soul->GetComponent<Soul>()->SetSoulType(mSoulType);

	// ��ġ ����
	auto localMat = GetComponent<game_module::Transform>()->GetLocalMatrix();
	localMat._42 += 1.f;
	soul->GetComponent<game_module::Transform>()->SetLocalMatrix(localMat);

	GetScene()->AddGameObject(soul);
	GetScene()->DestroyGameObject(GetGameObject());
}

void fq::client::Player::processCoolTime(float dt)
{
	mInvincibleElapsedTime = std::max(mInvincibleElapsedTime - dt, 0.f);
}

void fq::client::Player::processFeverTime(float dt)
{
	if (mFeverTime == 0.f)
		return;

	mFeverTime = std::max(mFeverTime - dt, 0.f);

	// ���� ���� ����
	if (mFeverTime == 0.f)
	{
		// TODO : ���� ������ ���� ������ ����
		mAttackPower = mAttackPower * 0.5f;
	}
}

float fq::client::Player::GetPlayerID() const
{
	return mController->GetControllerID();
}

