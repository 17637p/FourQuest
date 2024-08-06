
#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"

#include "GameManager.h"

// Player 
#include "PlayerDefine.h"
#include "Player.h"
#include "Soul.h"
#include "PlayerInputState.h"
#include "PlayerMovementState.h"
#include "DeadArmour.h"
#include "MagicArmour.h"
#include "MagicBallAttackState.h"
#include "AOEAttackState.h"
#include "LaserAttackState.h"
#include "ShiedlDashState.h"
#include "KnightArmour.h"
#include "SwordAttackState.h"
#include "ShieldAttackState.h"

// Monster
#include "Monster.h"
#include "MonsterIdle.h"
#include "MonsterFind.h"
#include "MonsterChase.h"
#include "MonsterDamaged.h"
#include "MonsterAttack.h"
#include "MonsterDie.h"

// MeleeMonster
#include "MeleeMonster.h"
#include "MeleeMonsterIdleState.h"
#include "MeleeMonsterAttackState.h"
#include "MeleeMonsterChaseState.h"
#include "MeleeMonsterPatrolState.h"
#include "MeleeMonsterDeadState.h"
#include "MeleeMonsterWaitAttackState.h"
#include "MeleeMonsterFindTargetState.h"
#include "MeleeMonsterHitState.h"
#include "MeleeMonsterExplosion.h"
#include "MeleeMonsterExplosionState.h"

// MeleeBossMonster;
#include "BossMonster.h"
#include "BossMonsterSmashDownState.h"
#include "BossMonsterChaseState.h"
#include "BossMonsterIdleState.h"
#include "BossMonsterRushState.h"
#include "BossMonsterFindTargetState.h"
#include "BossMonsterWaitAttackState.h"
#include "BossMonsterDeadState.h"
#include "BossMonsterWaitTargetState.h"
#include "BossMonsterHomingRushState.h"
#include "BossMonsterComboAttackState.h"
#include "BossMonsterPrepareAttackState.h"

// PlantMoster
#include "PlantMonster.h"
#include "LinearAttack.h"
#include "PlantMonsterAttckState.h"
#include "PlantMonsterDeadState.h"
#include "PlantMonsterIdleState.h"
#include "PlantMonsterStareState.h"

// MonsterSpawner
#include "MonsterSpawner.h"
#include "MonsterGroup.h"
#include "SpawnerDeadState.h"
#include "SpawnerOpenState.h"

#include "Attack.h"
#include "KnockBack.h"

// UI
#include "HpBar.h"
#include "PlayerUI.h"
#include "PlayerUIManager.h"
#include "BossHP.h"
#include "PauseUI.h"

#include "CameraMoving.h"

// Quest
#include "Quest.h"
#include "QuestManager.h"

void fq::client::RegisterMetaData()
{
	using namespace entt::literals;

	//////////////////////////////////////////////////////////////////////////
	//                             GameManager								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<GameManager>()
		.type("GameManager"_hs)
		.prop(reflect::prop::Name, "GameManager")
		.data<&GameManager::mPauseUI>("PauseUI"_hs)
		.prop(fq::reflect::prop::Name, "PauseUI")
		.base<game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             플레이어 관련								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<ESoulType>()
		.prop(fq::reflect::prop::Name, "SoulType")
		.conv<std::underlying_type_t<ESoulType>>()
		.data<ESoulType::Sword>("Sword"_hs) // 0
		.prop(fq::reflect::prop::Name, "Sword")
		.data<ESoulType::Shield>("Shield"_hs) // 1
		.prop(fq::reflect::prop::Name, "Shield")
		.data<ESoulType::Bow>("Bow"_hs) // 2
		.prop(fq::reflect::prop::Name, "Bow")
		.data<ESoulType::Staff>("Staff"_hs) // 3
		.prop(fq::reflect::prop::Name, "Staff");

	entt::meta<EArmourType>()
		.prop(fq::reflect::prop::Name, "ArmourType")
		.conv<std::underlying_type_t<EArmourType>>()
		.data<EArmourType::Knight>("Knight"_hs) // 0
		.prop(fq::reflect::prop::Name, "Knight")
		.data<EArmourType::Magic>("Magic"_hs) // 1
		.prop(fq::reflect::prop::Name, "Magic")
		.data<EArmourType::Warrior>("Warrior"_hs) // 2
		.prop(fq::reflect::prop::Name, "Warrior")
		.data<EArmourType::Archer>("Archer"_hs) // 3
		.prop(fq::reflect::prop::Name, "Archer");

	entt::meta<Player>()
		.type("Player"_hs)
		.prop(reflect::prop::Name, "Player")
		.prop(reflect::prop::Label, "Player")
		.data<&Player::SetSoulType, &Player::GetSoulType>("SoulType"_hs)
		.prop(reflect::prop::Name, "SoulType")
		.data<&Player::mArmourType>("ArmourType"_hs)
		.prop(reflect::prop::Name, "ArmourType")
		.data<&Player::mHp>("Hp"_hs)
		.prop(reflect::prop::Name, "Hp")
		.data<&Player::mAttackPower>("AttackPower"_hs)
		.prop(reflect::prop::Name, "AttackPower")
		.data<&Player::mInvincibleTime>("InvincibleTime"_hs)
		.prop(reflect::prop::Name, "InvincibleTime")
		.prop(reflect::prop::Comment, u8"무적시간")
		.data<&Player::mAttackPositionOffset>("FeverTime"_hs)
		.prop(reflect::prop::Name, "FeverTime")
		.prop(reflect::prop::Comment, u8"갑옷 버프 시간")
		.data<&Player::mAttackPrafab>("AttakPrefab"_hs)
		.prop(reflect::prop::Name, "AttakPrefab")
		.data<&Player::mSoulPrefab>("SoulPrefab"_hs)
		.prop(reflect::prop::Name, "SoulPrefab")
		.data<&Player::mAttackPositionOffset>("AttackPositionOffset"_hs)
		.prop(reflect::prop::Name, "AttackPositionOffset")
		.base<game_module::Component>();

	entt::meta<DeadArmour>()
		.type("DeadArmour"_hs)
		.prop(reflect::prop::Name, "DeadArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&DeadArmour::mLivingArmourPrefab>("LivingArmour"_hs)
		.prop(reflect::prop::Name, "LivingArmour")
		.base<game_module::Component>();

	entt::meta<MagicArmour>()
		.type("MagicArmour"_hs)
		.prop(reflect::prop::Name, "MagicArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&MagicArmour::mMagicBallSpeed>("MagicBallSpeed"_hs)
		.prop(reflect::prop::Name, "MagicBallSpeed")
		.data<&MagicArmour::mMagicBallPenetrationCount>("mMagicBallPenetrationCount"_hs)
		.prop(reflect::prop::Name, "mMagicBallPenetrationCount")
		.prop(reflect::prop::Comment, u8"매직볼트 관통횟수")
		.data<&MagicArmour::mAOEMoveRange>("AOEMoveRange"_hs)
		.prop(reflect::prop::Name, "AOEMoveRange")
		.data<&MagicArmour::mRazerDistance>("RazerDistance"_hs)
		.prop(reflect::prop::Name, "RazerDistance")
		.data<&MagicArmour::mRazerHiTick>("RazerHiTick"_hs)
		.prop(reflect::prop::Name, "RazerHiTick")
		.data<&MagicArmour::mMagicBall>("MagicBall"_hs)
		.prop(reflect::prop::Name, "MagicBall")
		.data<&MagicArmour::mAOE>("AOE"_hs)
		.prop(reflect::prop::Name, "AOE")
		.data<&MagicArmour::mAttackWarningUI>("AttackWarningUI"_hs)
		.prop(reflect::prop::Name, "AttackWarningUI")
		.data<&MagicArmour::mLaserEffect>("LaserEffect"_hs)
		.prop(reflect::prop::Name, "LaserEffect")
		.data<&MagicArmour::mLaserGatherEffect>("LaserGatherEffect"_hs)
		.prop(reflect::prop::Name, "LaserGatherEffect")
		.data<&MagicArmour::mLaserAttackBox>("LaserAttackBox"_hs)
		.prop(reflect::prop::Name, "LaserAttackBox")
		.base<game_module::Component>();

	entt::meta<KnightArmour>()
		.type("KnightArmour"_hs)
		.prop(reflect::prop::Name, "KnightArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&KnightArmour::mDashCoolTime>("DashCoolTime"_hs)
		.prop(reflect::prop::Name, "DashCoolTime")
		.data<&KnightArmour::mAttackOffset>("AttackOffset"_hs)
		.prop(reflect::prop::Name, "AttackOffset")
		.data<&KnightArmour::mShieldDashPower>("ShieldDashPushPower"_hs)
		.prop(reflect::prop::Name, "ShieldDashPushPower")
		.prop(reflect::prop::Comment, u8"A대쉬 전진하는 힘")
		.data<&KnightArmour::mXAttackDashPower>("XAttackPushPower"_hs)
		.prop(reflect::prop::Name, "XAttackPushPower")
		.prop(reflect::prop::Comment, u8"X공격 전진하는 힘")
		.data<&KnightArmour::mXAttackDashPower>("XAttackPushPower"_hs)
		.prop(reflect::prop::Name, "XAttackPushPower")
		.prop(reflect::prop::Comment, u8"X공격 전진하는 힘")
		.data<&KnightArmour::mSwordKnockBackPower>("SwordKnockBackPower"_hs)
		.prop(reflect::prop::Name, "SwordKnockBackPower")
		.prop(reflect::prop::Comment, u8"검 공격 넉백")
		.data<&KnightArmour::mShieldKnockPower>("ShieldKnockPower"_hs)
		.prop(reflect::prop::Name, "ShieldKnockPower")
		.prop(reflect::prop::Comment, u8"방패 공격 넉백")
		.data<&KnightArmour::mSwordAttack>("SwordAttack"_hs)
		.prop(reflect::prop::Name, "SwordAttack")
		.data<&KnightArmour::mSwordAttackEffect1>("SwordAttackEffect1"_hs)
		.prop(reflect::prop::Name, "SwordAttackEffect1")
		.data<&KnightArmour::mSwordAttackEffect2>("SwordAttackEffect2"_hs)
		.prop(reflect::prop::Name, "SwordAttackEffect2")
		.data<&KnightArmour::mShieldAttack>("ShieldAttack"_hs)
		.prop(reflect::prop::Name, "ShieldAttack")
		.data<&KnightArmour::mDashAttack>("DashAttack"_hs)
		.prop(reflect::prop::Name, "DashAttack")
		.base<game_module::Component>();

	entt::meta<Soul>()
		.type("Soul"_hs)
		.prop(reflect::prop::Name, "Soul")
		.prop(reflect::prop::Label, "Player")
		.data<&Soul::SetSoulType, &Soul::GetSoulType>("SoulType"_hs)
		.prop(reflect::prop::Name, "SoulType")
		.base<game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                         플레이어 상태 관련								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PlayerInputState>()
		.type("PlayerInputState"_hs)
		.prop(reflect::prop::Name, "PlayerInputState")
		.base<game_module::IStateBehaviour>();

	entt::meta<PlayerMovementState>()
		.type("PlayerMovementState"_hs)
		.prop(reflect::prop::Name, "PlayerMovementState")
		.data<&PlayerMovementState::mbCanMovePlayer>("CanMovePlayer"_hs)
		.prop(reflect::prop::Name, "CanMovePlayer")
		.data<&PlayerMovementState::mbOnRotation>("OnRotation"_hs)
		.prop(reflect::prop::Name, "OnRotation")
		.base<game_module::IStateBehaviour>();

	entt::meta<MagicBallAttackState>()
		.type("MagicAttackState"_hs)
		.prop(reflect::prop::Name, "MagicAttackState")
		.data<&MagicBallAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.prop(reflect::prop::Comment, u8"공격 시간")
		.base<game_module::IStateBehaviour>();

	entt::meta<AOEAttackState>()
		.type("AOEAttackState"_hs)
		.prop(reflect::prop::Name, "AOEAttackState")
		.base<game_module::IStateBehaviour>();

	entt::meta<LaserAttackState>()
		.type("RazerAttackState"_hs)
		.prop(reflect::prop::Name, "RazerAttackState")
		.data<&LaserAttackState::mLaserEmitTime>("LaserEmitTime"_hs)
		.prop(reflect::prop::Name, "LaserEmitTime")
		.base<game_module::IStateBehaviour>();

	entt::meta<ShiedlDashState>()
		.type("ShiedlDashState"_hs)
		.prop(reflect::prop::Name, "ShiedlDashState")
		.base<game_module::IStateBehaviour>();

	entt::meta<SwordAttackState>()
		.type("SwordAttackState"_hs)
		.prop(reflect::prop::Name, "SwordAttackState")
		.data<&SwordAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.base<game_module::IStateBehaviour>();

	entt::meta<ShieldAttackState>()
		.type("ShieldAttackState"_hs)
		.prop(reflect::prop::Name, "ShieldAttackState")
		.data<&ShieldAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.base<game_module::IStateBehaviour>();


	//////////////////////////////////////////////////////////////////////////
	//                             몬스터									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Monster>()
		.type("Monster"_hs)
		.prop(fq::reflect::prop::Name, "Monster")
		.prop(reflect::prop::Label, "Monster")
		.data<&Monster::SetHP, &Monster::GetHP>("HP"_hs)
		.prop(fq::reflect::prop::Name, "HP")
		.data<&Monster::SetAttackPower, &Monster::GetAttackPower>("AttackPower"_hs)
		.prop(fq::reflect::prop::Name, "AttackPower")
		.data<&Monster::SetMoveSpeed, &Monster::GetMoveSpeed>("MaxMoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MaxMoveSpeed")
		.data<&Monster::SetAcceleration, &Monster::GetAcceleration>("MaxAcceleration"_hs)
		.prop(fq::reflect::prop::Name, "MaxAcceleration")
		.data<&Monster::SetChaseDistance, &Monster::GetChaseDistance>("ChaseDistance"_hs)
		.prop(fq::reflect::prop::Name, "ChaseDistance")
		.data<&Monster::SetAttackWaitTime, &Monster::GetAttackWaitTime>("AttackWaitTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackWaitTime")
		.data<&Monster::SetTargetAttackRange, &Monster::GetTargetAttackRange>("TargetAttackRange"_hs)
		.prop(fq::reflect::prop::Name, "TargetAttackRange")
		.data<&Monster::SetDamagedDelay, &Monster::GetDamagedDelay>("DamagedDelay"_hs)
		.prop(fq::reflect::prop::Name, "DamagedDelay")
		.data<&Monster::SetAttack, &Monster::GetAttack>("Attack"_hs)
		.prop(fq::reflect::prop::Name, "Attack")
		.base<fq::game_module::Component>();


	//////////////////////////////////////////////////////////////////////////
	//                             몬스터 상태								//
	//////////////////////////////////////////////////////////////////////////
	entt::meta<MonsterIdle>()
		.type("MonsterIdle"_hs)
		.prop(fq::reflect::prop::Name, "MonsterIdle")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MonsterFind>()
		.type("MonsterFind"_hs)
		.prop(fq::reflect::prop::Name, "MonsterFind")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MonsterChase>()
		.type("MonsterChase"_hs)
		.prop(fq::reflect::prop::Name, "MonsterChase")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MonsterDamaged>()
		.type("MonsterDamaged"_hs)
		.prop(fq::reflect::prop::Name, "MonsterDamaged")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MonsterAttack>()
		.type("MonsterAttack"_hs)
		.prop(fq::reflect::prop::Name, "MonsterAttack")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MonsterDie>()
		.type("MonsterDie"_hs)
		.prop(fq::reflect::prop::Name, "MonsterDie")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             근접 몬스터 	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<MeleeMonster>()
		.type("MeleeMonster"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonster")
		.prop(reflect::prop::Label, "Monster")
		.data<&MeleeMonster::mHp>("Hp"_hs)
		.prop(fq::reflect::prop::Name, "Hp")
		.data<&MeleeMonster::mAttackPower>("AttackPower"_hs)
		.prop(fq::reflect::prop::Name, "AttackPower")
		.data<&MeleeMonster::mAttackCoolTime>("AttackCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackCoolTime")
		.data<&MeleeMonster::mAttackOffset>("AttackOffset"_hs)
		.prop(fq::reflect::prop::Name, "AttackOffset")
		.data<&MeleeMonster::mAcceleration>("Acceleration"_hs)
		.prop(fq::reflect::prop::Name, "Acceleration")
		.data<&MeleeMonster::mMoveSpeed>("MoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MoveSpeed")
		.data<&MeleeMonster::mAttackRange>("AttackRange"_hs)
		.prop(fq::reflect::prop::Name, "AttackRange")
		.data<&MeleeMonster::mAttackPrefab>("AttackPrefab"_hs)
		.prop(fq::reflect::prop::Name, "AttackPrefab")
		.data<&MeleeMonster::mAttackEffect>("AttackEffect"_hs)
		.prop(fq::reflect::prop::Name, "AttackEffect")
		.data<&MeleeMonster::mPatrolRange>("PatrolRange"_hs)
		.prop(fq::reflect::prop::Name, "PatrolRange")
		.prop(fq::reflect::prop::Comment, u8"순찰 범위")
		.data<&MeleeMonster::mDetectRange>("DetectRange"_hs)
		.prop(fq::reflect::prop::Name, "DetectRange")
		.prop(fq::reflect::prop::Comment, u8"플레이어 감지 범위")
		.base<fq::game_module::Component>();

	entt::meta<MeleeMonsterExplosion>()
		.type("MeleeMonsterExplosion"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterExplosion")
		.prop(reflect::prop::Label, "Monster")
		.data<&MeleeMonsterExplosion::mExplosionRadius>("ExplosionRadius"_hs)
		.prop(fq::reflect::prop::Name, "ExplosionRadius")
		.data<&MeleeMonsterExplosion::mExplosionDamage>("ExplosionDamage"_hs)
		.prop(fq::reflect::prop::Name, "ExplosionDamage")
		.data<&MeleeMonsterExplosion::mExplosionTime>("ExplosionTime"_hs)
		.prop(fq::reflect::prop::Name, "ExplosionTime")
		.data<&MeleeMonsterExplosion::mWarningUI>("WarningUI"_hs)
		.prop(fq::reflect::prop::Name, "WarningUI")
		.data<&MeleeMonsterExplosion::mExplosion>("Explosion"_hs)
		.prop(fq::reflect::prop::Name, "Explosion")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             근접 몬스터 상태  							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<MeleeMonsterIdleState>()
		.type("MeleeMonsterIdleState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterIdleState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterAttackState>()
		.type("MeleeMonsterAttackState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterAttackState")
		.data<&MeleeMonsterAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(fq::reflect::prop::Name, "AttackTiming")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterChaseState>()
		.type("MeleeMonsterChaseState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterChaseState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterPatrolState>()
		.type("MeleeMonsterPatrolState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterPatrolState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterDeadState>()
		.type("MeleeMonsterDeadState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterDeadState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterWaitAttackState>()
		.type("MeleeMonsterWaitAttackState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterWaitAttackState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterFindTargetState>()
		.type("MeleeMonsterFindTargetState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterFindTargetState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterHitState>()
		.type("MeleeMonsterHitState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterHitState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<MeleeMonsterExplosionState>()
		.type("MeleeMonsterExplosionState"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterExplosionState")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             보스 몬스터 	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<BossMonster>()
		.type("BossMonster"_hs)
		.prop(fq::reflect::prop::Name, "BossMonster")
		.prop(reflect::prop::Label, "Monster")
		.data<&BossMonster::mHp>("Hp"_hs)
		.prop(fq::reflect::prop::Name, "Hp")
		.data<&BossMonster::mAttackPower>("AttackPower"_hs)
		.prop(fq::reflect::prop::Name, "AttackPower")
		.data<&BossMonster::mAttackCoolTime>("AttackCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackCoolTime")
		.data<&BossMonster::mSmashDownOffset>("SmashDownOffset"_hs)
		.prop(fq::reflect::prop::Name, "SmashDownOffset")
		.data<&BossMonster::mComboAttackOffset>("ComboAttackOffset"_hs)
		.prop(fq::reflect::prop::Name, "ComboAttackOffset")
		.data<&BossMonster::mAcceleration>("Acceleration"_hs)
		.prop(fq::reflect::prop::Name, "Acceleration")
		.data<&BossMonster::mMoveSpeed>("MoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MoveSpeed")
		.data<&BossMonster::mRushPower>("RushPower"_hs)
		.prop(fq::reflect::prop::Name, "RushPower")
		.data<&BossMonster::mComboAttackReboundPower>("ComboAttackReboundPower"_hs)
		.prop(fq::reflect::prop::Name, "ComboAttackReboundPower")
		.data<&BossMonster::mAttackRange>("AttackRange"_hs)
		.prop(fq::reflect::prop::Name, "AttackRange")
		.data<&BossMonster::mDetectRange>("DetectRange"_hs)
		.prop(fq::reflect::prop::Name, "DetectRange")
		.data<&BossMonster::mRotationSpeed>("RotationSpeed"_hs)
		.prop(fq::reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"플레이어 감지 범위")
		.data<&BossMonster::mSmashDownAttack>("SmashDownAttack"_hs)
		.prop(fq::reflect::prop::Name, "SmashDownAttack")
		.data<&BossMonster::mSmashDownEffect>("SmashDownEffect"_hs)
		.prop(fq::reflect::prop::Name, "SmashDownEffect")
		.data<&BossMonster::mRushAttack>("RushAttack"_hs)
		.prop(fq::reflect::prop::Name, "RushAttack")
		.data<&BossMonster::mRushEffect>("RushEffect"_hs)
		.prop(fq::reflect::prop::Name, "RushEffect")
		.data<&BossMonster::mComboAttack>("ComboAttack"_hs)
		.prop(fq::reflect::prop::Name, "ComboAttack")
		.data<&BossMonster::mComboEffect>("ComboEffect"_hs)
		.prop(fq::reflect::prop::Name, "ComboEffect")
		.data<&BossMonster::mHpBarPrefab>("HpBarPrefab"_hs)
		.prop(fq::reflect::prop::Name, "HpBarPrefab")
		.base<fq::game_module::Component>();

	entt::meta<BossMonsterIdleState>()
		.type("BossMonsterIdleState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterIdleState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterSmashDownState>()
		.type("BossMonsterSmashDownState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterSmashDownState")
		.data<&BossMonsterSmashDownState::mAttackEmitTime>("AttackEmitTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackEmitTime")
		.data<&BossMonsterSmashDownState::mEffectEmitTime>("EffectEmitTime"_hs)
		.prop(fq::reflect::prop::Name, "EffectEmitTime")
		.data<&BossMonsterSmashDownState::mHomingTime>("HomingTime"_hs)
		.prop(fq::reflect::prop::Name, "HomingTime")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterChaseState>()
		.type("BossMonsterChaseState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterChaseState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterRushState>()
		.type("BossMonsterRushState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterRushState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterFindTargetState>()
		.type("BossMonsterFindTargetState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterFindTargetState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterWaitAttackState>()
		.type("BossMonsterWaitAttackState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterWaitAttackState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterDeadState>()
		.type("BossMonsterDeadState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterDeadState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterWaitTargetState>()
		.type("BossMonsterWaitTargetState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterWaitTargetState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterHomingRushState>()
		.type("BossMonsterHomingRushState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterHomingRushState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterComboAttackState>()
		.type("BossMonsterComboAttackState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterComboAttackState")
		.data<&BossMonsterComboAttackState::mEmitAttackTime>("EmitAttackTime"_hs)
		.prop(fq::reflect::prop::Name, "EmitAttackTime")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterPrepareAttackState>()
		.type("BossMonsterPrepareAttackState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterPrepareAttackState")
		.data<&BossMonsterPrepareAttackState::mHomingTime>("HomingTime"_hs)
		.prop(fq::reflect::prop::Name, "HomingTime")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             원거리 몬스터 	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PlantMonster>()
		.type("PlantMonster"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonster")
		.prop(reflect::prop::Label, "Monster")
		.data<&PlantMonster::mHp>("Hp"_hs)
		.prop(fq::reflect::prop::Name, "Hp")
		.data<&PlantMonster::mAttackPower>("AttackPower"_hs)
		.prop(fq::reflect::prop::Name, "AttackPower")
		.data<&PlantMonster::mProjectileSpeed>("ProjectileSpeed"_hs)
		.prop(fq::reflect::prop::Name, "ProjectileSpeed")
		.data<&PlantMonster::mAttackRange>("AttackRange"_hs)
		.prop(fq::reflect::prop::Name, "AttackRange")
		.data<&PlantMonster::mAttackPrefab>("AttackPrefab"_hs)
		.prop(fq::reflect::prop::Name, "AttackPrefab")
		.data<&PlantMonster::mAttackCoolTime>("AttackCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackCoolTime")
		.data<&PlantMonster::mRotationSpeed>("RotationSpeed"_hs)
		.prop(fq::reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"회전 속도 0 < x <= 1.f")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                         원거리 몬스터 상태								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PlantMonsterAttckState>()
		.type("PlantMonsterAttckState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterAttckState")
		.data<&PlantMonsterAttckState::mAttackTiming>("AttackTiming"_hs)
		.prop(fq::reflect::prop::Name, "AttackTiming")
		.data<&PlantMonsterAttckState::mLookAtTime>("LookAtTime"_hs)
		.prop(fq::reflect::prop::Name, "LookAtTime")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<PlantMonsterStareState>()
		.type("PlantMonsterStareState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterStareState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<PlantMonsterDeadState>()
		.type("PlantMonsterDeadState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterDeadState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<PlantMonsterIdleState>()
		.type("PlantMonsterIdleState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterIdleState")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             몬스터 그룹	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<MonsterGroup>()
		.type("MonsterGroup"_hs)
		.prop(fq::reflect::prop::Name, "MonsterGroup")
		.prop(reflect::prop::Label, "Monster")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             몬스터 스포너	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<MonsterSpawner>()
		.type("MonsterSpawner"_hs)
		.prop(fq::reflect::prop::Name, "MonsterSpawner")
		.prop(reflect::prop::Label, "Monster")
		.data<&MonsterSpawner::mMonster>("MonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "MonsterPrefab")
		.data<&MonsterSpawner::mSpawnCoolTime>("SpwanCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "SpwanCoolTime")
		.data<&MonsterSpawner::mSpawnOffset>("SpawnOffeset"_hs)
		.prop(fq::reflect::prop::Name, "SpawnOffeset")
		.base<fq::game_module::Component>();

	entt::meta<SpawnerOpenState>()
		.type("SpawnerOpenState"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerOpenState")
		.data<&SpawnerOpenState::mSpwanTiming>("SpwanTiming"_hs)
		.prop(fq::reflect::prop::Name, "SpwanTiming")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<SpawnerDeadState>()
		.type("SpawnerDeadState"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerDeadState")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             카메라									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<CameraMoving>()
		.type("CameraMoving"_hs)
		.prop(fq::reflect::prop::Name, "CameraMoving")
		.data<&CameraMoving::SetMoveSpeed, &CameraMoving::GetMoveSpeed>("MoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MoveSpeed")
		.data<&CameraMoving::SetZoomSpeed, &CameraMoving::GetZoomSpeed>("ZoomSpeed"_hs)
		.prop(fq::reflect::prop::Name, "ZoomSpeed")
		.data<&CameraMoving::SetZoomMin, &CameraMoving::GetZoomMin>("ZoomMin"_hs)
		.prop(fq::reflect::prop::Name, "ZoomMin")
		.data<&CameraMoving::SetZoomMax, &CameraMoving::GetZoomMax>("ZoomMax"_hs)
		.prop(fq::reflect::prop::Name, "ZoomMax")
		.data<&CameraMoving::SetZoomOutPadX, &CameraMoving::GetZoomOutPadX>("ZoomOutPadX"_hs)
		.prop(fq::reflect::prop::Name, "ZoomOutPadX")
		.data<&CameraMoving::SetZoomOutPadY, &CameraMoving::GetZoomOutPadY>("ZoomOutPadY"_hs)
		.prop(fq::reflect::prop::Name, "ZoomOutPadY")
		.data<&CameraMoving::SetZoomInPadX, &CameraMoving::GetZoomInPadX>("ZoomInPadX"_hs)
		.prop(fq::reflect::prop::Name, "ZoomInPadX")
		.data<&CameraMoving::SetZoomInPadY, &CameraMoving::GetZoomInPadY>("ZoomInPadY"_hs)
		.prop(fq::reflect::prop::Name, "ZoomInPadY")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                            공격										//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Attack>()
		.type("Attack"_hs)
		.prop(fq::reflect::prop::Name, "Attack")
		.data<&Attack::mDestroyTime>("DestroyTime"_hs)
		.prop(fq::reflect::prop::Name, "DestroyTime")
		.base<fq::game_module::Component>();

	entt::meta<LinearAttack>()
		.type("LinearAttack"_hs)
		.prop(fq::reflect::prop::Name, "LinearAttack")
		.base<fq::game_module::Component>();

	entt::meta<KnockBack>()
		.type("KnockBack"_hs)
		.prop(fq::reflect::prop::Name, "KnockBack")
		.data<&KnockBack::mVelocity>("Velocity"_hs)
		.prop(fq::reflect::prop::Name, "Velocity")
		.data<&KnockBack::mFriction>("Friction"_hs)
		.prop(fq::reflect::prop::Name, "Friction")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             UI										//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<HpBar>()
		.type("HpBar"_hs)
		.prop(fq::reflect::prop::Name, "HpBar")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&HpBar::mIsVisible>("IsVisible"_hs)
		.prop(fq::reflect::prop::Name, "IsVisible")
		.data<&HpBar::mBarSize>("BarSize"_hs)
		.prop(fq::reflect::prop::Name, "BarSize")
		.data<&HpBar::mDecreaseSpeed>("DecreaseSpeed"_hs)
		.prop(fq::reflect::prop::Name, "DecreaseSpeed")
		.data<&HpBar::mDecreaseOffset>("DecreaseOffset"_hs)
		.prop(fq::reflect::prop::Name, "DecreaseOffset")
		.data<&HpBar::mWorldOffset>("WorldOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldOffset")
		.prop(fq::reflect::prop::Comment, u8"월드 공간의 Y를 더한후 UI 위치 계산")
		.data<&HpBar::mScreenOffset>("ScreenOffset"_hs)
		.prop(fq::reflect::prop::Name, "ScreenOffset")
		.prop(fq::reflect::prop::Comment, u8"Screen 공간의 Y를 더한후 UI 위치 계산")
		.data<&HpBar::mInnerOffset>("InnerOffset"_hs)
		.prop(fq::reflect::prop::Name, "InnerOffset")
		.prop(fq::reflect::prop::Comment, u8"Bar 외부와 내부의 크기 차이")
		.base<fq::game_module::Component>();

	entt::meta<PlayerUI>()
		.type("PlayerUI"_hs)
		.prop(fq::reflect::prop::Name, "PlayerUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&PlayerUI::mPlayerID>("PlayerID"_hs)
		.prop(fq::reflect::prop::Name, "PlayerID")
		.prop(fq::reflect::prop::Comment, u8"ControllerID와 맞춰줄 것 0~3")
		.base<fq::game_module::Component>();

	entt::meta<PlayerUIManager>()
		.type("PlayerUIManager"_hs)
		.prop(fq::reflect::prop::Name, "PlayerUIManager")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&PlayerUIManager::mPlayerUIPrefab>("PlayerUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "PlayerUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<BossHP>()
		.type("BossHP"_hs)
		.prop(fq::reflect::prop::Name, "BossHP")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<PauseUI>()
		.type("PauseUI"_hs)
		.prop(fq::reflect::prop::Name, "PauseUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                             Monster Type								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<EMonsterType>()
		.prop(fq::reflect::prop::Name, "MonsterType")
		.conv<std::underlying_type_t<EMonsterType>>()
		.data<EMonsterType::Melee>("Melee"_hs) // 0
		.prop(fq::reflect::prop::Name, "Melee")
		.data<EMonsterType::Explosion>("Explosion"_hs) // 1
		.prop(fq::reflect::prop::Name, "Explosion")
		.data<EMonsterType::Boss>("Boss"_hs) // 2
		.prop(fq::reflect::prop::Name, "Boss")
		.data<EMonsterType::Plant>("Plant"_hs) // 3
		.prop(fq::reflect::prop::Name, "Plant")
		.data<EMonsterType::Spawner>("Spawner"_hs) // 3
		.prop(fq::reflect::prop::Name, "Spawner")
		.data<EMonsterType::All>("All"_hs) // 3
		.prop(fq::reflect::prop::Name, "All");

	//////////////////////////////////////////////////////////////////////////
	//                             Quest									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PreQuest>()
		.type("PreQuest"_hs)
		.prop(fq::reflect::prop::Name, "PreQuest")
		.prop(fq::reflect::prop::POD)
		.data<&PreQuest::preIndex>("PreIndex"_hs)
		.prop(fq::reflect::prop::Name, "PreIndex")
		.data<&PreQuest::preIsMain>("PreIsMain"_hs)
		.prop(fq::reflect::prop::Name, "PreIsMain");

	entt::meta<QuestColliderTrigger>()
		.type("QuestColliderTrigger"_hs)
		.prop(fq::reflect::prop::Name, "QuestColliderTrigger")
		.prop(fq::reflect::prop::POD)
		.data<&QuestColliderTrigger::colliderName>("ColliderName"_hs)
		.prop(fq::reflect::prop::Name, "ColliderName")
		.prop(reflect::prop::Comment, u8"Collider를 가진 오브젝트 이름")
		.data<&QuestColliderTrigger::isAll>("IsAll"_hs)
		.prop(fq::reflect::prop::Name, "IsAll");

	entt::meta<MonsterKill>()
		.type("MonsterKill"_hs)
		.prop(fq::reflect::prop::Name, "MonsterKill")
		.prop(fq::reflect::prop::POD)
		.data<&MonsterKill::monsterType>("MonsterType"_hs)
		.prop(fq::reflect::prop::Name, "MonsterType")
		.data<&MonsterKill::requestsNumber>("RequestsNumber"_hs)
		.prop(fq::reflect::prop::Name, "RequestsNumber");

	entt::meta<MonsterGroupKill>()
		.type("MonsterGroupKill"_hs)
		.prop(fq::reflect::prop::Name, "MonsterGroupKill")
		.prop(fq::reflect::prop::POD)
		.data<&MonsterGroupKill::groupIndex>("GroupIndex"_hs)
		.prop(fq::reflect::prop::Name, "GroupIndex");

	entt::meta<Defence>()
		.type("Defence"_hs)
		.prop(fq::reflect::prop::Name, "Defence")
		.prop(fq::reflect::prop::POD)
		.data<&Defence::colliderName>("ColliderName"_hs)
		.prop(fq::reflect::prop::Name, "ColliderName")
		.prop(reflect::prop::Comment, u8"Collider를 가진 오브젝트 이름")
		.data<&Defence::seconds>("Seconds"_hs)
		.prop(fq::reflect::prop::Name, "Seconds");

	entt::meta<ClearQuest>()
		.type("ClearQuest"_hs)
		.prop(fq::reflect::prop::Name, "ClearQuest")
		.prop(fq::reflect::prop::POD)
		.data<&ClearQuest::clearIndex>("ClearQuestIndex"_hs)
		.prop(fq::reflect::prop::Name, "ClearQuestIndex")
		.data<&ClearQuest::clearIsMain>("ClearQuestIsMain"_hs)
		.prop(fq::reflect::prop::Name, "ClearQuestIsMain");

	entt::meta<ObjectInteraction>()
		.type("ObjectInteraction"_hs)
		.prop(fq::reflect::prop::Name, "ObjectInteraction")
		.prop(fq::reflect::prop::POD)
		.data<&ObjectInteraction::tag>("Tag"_hs)
		.prop(fq::reflect::prop::Name, "Tag");

	entt::meta<QuestJoinCondition>()
		.type("QuestJoinCondition"_hs)
		.prop(fq::reflect::prop::Name, "QuestJoinCondition")
		.prop(fq::reflect::prop::POD)
		.data<&QuestJoinCondition::preQuestList>("PreQuestList"_hs)
		.prop(fq::reflect::prop::Name, "PreQuestList")
		.data<&QuestJoinCondition::colliderTriggerList>("ColliderTriggerList"_hs)
		.prop(fq::reflect::prop::Name, "ColliderTriggerList");

	entt::meta<QuestClearCondition>()
		.type("QuestClearCondition"_hs)
		.prop(fq::reflect::prop::Name, "QuestClearCondition")
		.prop(fq::reflect::prop::POD)
		.data<&QuestClearCondition::monsterKillList>("MonsterKillList"_hs)
		.prop(fq::reflect::prop::Name, "MonsterKillList")
		.data<&QuestClearCondition::monsterGroupKillList>("MonsterGroupKillList"_hs)
		.prop(fq::reflect::prop::Name, "MonsterGroupKillList")
		.data<&QuestClearCondition::defenceList>("DefenceList"_hs)
		.prop(fq::reflect::prop::Name, "DefenceList")
		.data<&QuestClearCondition::clearQuestList>("ClearQuestList"_hs)
		.prop(fq::reflect::prop::Name, "ClearQuestList")
		.data<&QuestClearCondition::colliderTriggerList>("ColliderTriggerList"_hs)
		.prop(fq::reflect::prop::Name, "ColliderTriggerList")
		.data<&QuestClearCondition::objectInteration>("ObjectInteration"_hs)
		.prop(fq::reflect::prop::Name, "ObjectInteration");

	entt::meta<Quest>()
		.type("Quest"_hs)
		.prop(fq::reflect::prop::Name, "Quest")
		.prop(fq::reflect::prop::POD)
		.data<&Quest::mIndex>("Index"_hs)
		.prop(fq::reflect::prop::Name, "Index")
		.data<&Quest::mName>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name")
		.data<&Quest::mIsMain>("IsMain"_hs)
		.prop(fq::reflect::prop::Name, "IsMain")
		.data<&Quest::mJoinConditionList>("JoinConditionList"_hs)
		.prop(fq::reflect::prop::Name, "JoinConditionList")
		.data<&Quest::mclearConditionList>("ClearConditionList"_hs)
		.prop(fq::reflect::prop::Name, "ClearConditionList");

	entt::meta<StartSubQuestIndex>()
		.type("StartSubQuestIndex"_hs)
		.prop(fq::reflect::prop::Name, "StartSubQuestIndex")
		.prop(fq::reflect::prop::POD)
		.data<&StartSubQuestIndex::index>("Index"_hs)
		.prop(fq::reflect::prop::Name, "Index");

	entt::meta<StartQuests>()
		.type("StartQuests"_hs)
		.prop(fq::reflect::prop::Name, "StartQuests")
		.prop(fq::reflect::prop::POD)
		.data<&StartQuests::startMainQuestIndex>("StartMainQuestIndex"_hs)
		.prop(fq::reflect::prop::Name, "StartMainQuestIndex")
		.data<&StartQuests::startSubQuestIndex>("StartSubQuestIndex"_hs)
		.prop(fq::reflect::prop::Name, "StartSubQuestIndex");

	entt::meta<QuestManager>()
		.type("QuestManager"_hs)
		.prop(fq::reflect::prop::Name, "QuestManager")
		//.data<&QuestManager::mCurMainQuest>("CurMainQuest"_hs)
		//.prop(fq::reflect::prop::Name, "CurMainQuest")
		//.data<&QuestManager::mCurSubQuest>("CurSubQuest"_hs)
		//.prop(fq::reflect::prop::Name, "CurSubQuest")
		.data<&QuestManager::mStartQuests>("StartQuests"_hs)
		.prop(fq::reflect::prop::Name, "StartQuests")
		.data<&QuestManager::mMainQuests>("MainQuest"_hs)
		.prop(fq::reflect::prop::Name, "MainQuest")
		.data<&QuestManager::mSubQuests>("SubQuest"_hs)
		.prop(fq::reflect::prop::Name, "SubQuest")
		.base<fq::game_module::Component>();
}
