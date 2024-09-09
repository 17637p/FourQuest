#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"

#include "GameManager.h"
#include "MaterialManager.h"
#include "MonsterManager.h"

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
#include "ShieldDashState.h"
#include "KnightArmour.h"
#include "SwordAttackState.h"
#include "ShieldAttackState.h"
#include "StaffSoulAttackState.h"
#include "SwordSoulAttackState.h"
#include "BowSoulAttackState.h"
#include "AxeSoulAttackState.h"

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
#include "PlantMonsterHitState.h"
#include "PlantMonsterIdleState.h"
#include "PlantMonsterStareState.h"

// MonsterSpawner
#include "MonsterSpawner.h"
#include "MonsterGroup.h"
#include "SpawnerDeadState.h"
#include "SpawnerOpenState.h"

#include "Attack.h"
#include "StaffSoulAttack.h"
#include "KnockBack.h"

// UI
#include "HpBar.h"
#include "PlayerUI.h"
#include "PlayerUIManager.h"
#include "BossHP.h"
#include "PauseUI.h"
#include "LoadingUI.h"
#include "TitleUI.h"
#include "SoulSelectUI.h"
#include "SettingUI.h"
#include "RepauseUI.h"

#include "CameraMoving.h"

// Quest
#include "Quest.h"
#include "QuestManager.h"
#include "DefenceCounter.h"
#include "QuestColliderTriggerChecker.h"
#include "ArmourSet.h"
#include "ArmourSpawner.h"

// MonsterSpawner
#include "SpawnerGroup.h"
#include "SpawnStruct.h"
#include "Spawner.h"

// GameVariable
#include "PlayerSoulVariable.h"
#include "DamageVariable.h"
#include "SettingVariable.h"
#include "PlayerInfoVariable.h"

// Box
#include "Box.h"

// etc
#include "BGM.h"
#include "Portal.h"

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

	entt::meta<TestPOD>()
		.type("TestPOD"_hs)	
		.prop(reflect::prop::Name, "TestPOD")
		.prop(reflect::prop::POD)
		.data<&TestPOD::res>("res"_hs)
		.prop(fq::reflect::prop::Name, "res");

	entt::meta<MonsterManager>()
		.type("MonsterManager"_hs)
		.prop(reflect::prop::Name, "MonsterManager")
		.data<&MonsterManager::SetMonsterSize, &MonsterManager::GetMonsterSize>("Size"_hs)
		.prop(fq::reflect::prop::Name, "Size")
		.data<&MonsterManager::mRes>("mRes"_hs)
		.prop(fq::reflect::prop::Name, "mRes")
		.base<game_module::Component>();


	//////////////////////////////////////////////////////////////////////////
	//                             ETC										//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Portal>()
		.type("Portal"_hs)
		.prop(reflect::prop::Name, "Portal")
		.data<&Portal::mNextSceneName>("NextSceneName"_hs)
		.prop(fq::reflect::prop::Name, "NextSceneName")
		.base<game_module::Component>();

	entt::meta<BGM>()
		.type("BGM"_hs)
		.prop(reflect::prop::Name, "BGM")
		.data<&BGM::mSoundKey>("SoundKey"_hs)
		.prop(fq::reflect::prop::Name, "SoundKey")
		.base<game_module::Component>();

	entt::meta<MaterialManager>()
		.type("MaterialManager"_hs)
		.prop(reflect::prop::Name, "MaterialManager")
		.data<&MaterialManager::mManagerMaterialPath>("MaterialPath"_hs)
		.prop(fq::reflect::prop::Name, "MaterialPath")
		.prop(reflect::prop::Comment, u8"플레이어와 카메라 사이에 가려질 시 사용할 머터리얼")
		.prop(fq::reflect::prop::DragDrop, ".material")
		.data<&MaterialManager::mLocalPointOffset>("LocalPointOffset"_hs)
		.prop(fq::reflect::prop::Name, "LocalPointOffset")
		.prop(reflect::prop::Comment, u8"플레이어 반직선 로컬 오프셋")
		.data<&MaterialManager::mWorldPointOffset>("WorldPointOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldPointOffset")
		.prop(reflect::prop::Comment, u8"플레이어 반직선 월드 오프셋")
		.data<&MaterialManager::mbUseDebugDraw>("UseDebugDraw"_hs)
		.prop(fq::reflect::prop::Name, "UseDebugDraw")
		.prop(reflect::prop::Comment, u8"디버깅 정보 사용 여부")
		.base<game_module::Component>();


	//////////////////////////////////////////////////////////////////////////
	//                             플레이어 관련								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<ESoulType>()
		.prop(fq::reflect::prop::Name, "SoulType")
		.conv<std::underlying_type_t<ESoulType>>()
		.data<ESoulType::Sword>("Sword"_hs) // 0
		.prop(fq::reflect::prop::Name, "Sword")
		.data<ESoulType::Axe>("Axe"_hs) // 1
		.prop(fq::reflect::prop::Name, "Axe")
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
		.data<&Player::mFeverTime>("FeverTime"_hs)
		.prop(reflect::prop::Name, "FeverTime")
		.prop(reflect::prop::Comment, u8"피버 타임")
		.data<&Player::mStaffSoulAttack>("StaffSoulAttack"_hs)
		.prop(reflect::prop::Name, "StaffSoulAttack")
		.data<&Player::mSwordSoulAttack>("SwordSoulAttack"_hs)
		.prop(reflect::prop::Name, "SwordSoulAttack")
		.data<&Player::mBowSoulAttack>("BowSoulAttack"_hs)
		.prop(reflect::prop::Name, "BowSoulAttack")
		.data<&Player::mAxeSoulAttack>("AxeSoulAttack"_hs)
		.prop(reflect::prop::Name, "AxeSoulAttack")
		.data<&Player::mSoulPrefab>("SoulPrefab"_hs)
		.prop(reflect::prop::Name, "SoulPrefab")
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
		.data<&MagicArmour::mLaserDistance>("RazerDistance"_hs)
		.prop(reflect::prop::Name, "RazerDistance")
		.data<&MagicArmour::mLaserHiTick>("RazerHiTick"_hs)
		.prop(reflect::prop::Name, "RazerHiTick")
		.data<&MagicArmour::mMagicBall>("MagicBall"_hs)
		.prop(reflect::prop::Name, "MagicBall")
		.data<&MagicArmour::mAOE>("AOE"_hs)
		.prop(reflect::prop::Name, "AOE")
		.data<&MagicArmour::mAttackWarningUI>("AttackWarningUI"_hs)
		.prop(reflect::prop::Name, "AttackWarningUI")
		.data<&MagicArmour::mLaserHeadEffectPrefab>("LaserHeadEffect"_hs)
		.prop(reflect::prop::Name, "LaserHeadEffect")
		.data<&MagicArmour::mLaserLineEffectPrefab>("LaserTailEffect"_hs)
		.prop(reflect::prop::Name, "LaserTailEffect")
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
		.data<&Soul::mSwordColor>("SwordColor"_hs)
		.prop(reflect::prop::Name, "SwordColor")
		.data<&Soul::mStaffColor>("StaffColor"_hs)
		.prop(reflect::prop::Name, "StaffColor")
		.data<&Soul::mAxeColor>("AxeColor"_hs)
		.prop(reflect::prop::Name, "AxeColor")
		.data<&Soul::mBowColor>("BowColor"_hs)
		.prop(reflect::prop::Name, "BowColor")
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

	entt::meta<ShieldDashState>()
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

	entt::meta<StaffSoulAttackState>()
		.type("StaffSoulAttackState"_hs)
		.prop(reflect::prop::Name, "StaffSoulAttackState")
		.base<game_module::IStateBehaviour>();

	entt::meta<SwordSoulAttackState>()
		.type("SwordSoulAttackState"_hs)
		.prop(reflect::prop::Name, "SwordSoulAttackState")
		.data<&SwordSoulAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.base<game_module::IStateBehaviour>();

	entt::meta<BowSoulAttackState>()
		.type("BowSoulAttackState"_hs)
		.prop(reflect::prop::Name, "BowSoulAttackState")
		.base<game_module::IStateBehaviour>();

	entt::meta<AxeSoulAttackState>()
		.type("AxeSoulAttackState"_hs)
		.prop(reflect::prop::Name, "AxeSoulAttackState")
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

	entt::meta<PlantMonsterHitState>()
		.type("PlantMonsterHitState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterHitState")
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
		.data<&MonsterGroup::mMonsterCount>("MonsterCount"_hs)
		.prop(fq::reflect::prop::Name, "MonsterCount")
		.data<&MonsterGroup::mGroupIndex>("GroupIndex"_hs)
		.prop(fq::reflect::prop::Name, "GroupIndex")
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
		.data<&CameraMoving::mMoveSpeed>("MoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MoveSpeed")
		.prop(fq::reflect::prop::Comment, u8"카메라 움직이는 속도")
		.data<&CameraMoving::mZoomSpeed>("ZoomSpeed"_hs)
		.prop(fq::reflect::prop::Name, "ZoomSpeed")
		.prop(fq::reflect::prop::Comment, u8"카메라 ZoomInOut 속도")
		.data<&CameraMoving::mZoomMin>("ZoomMin"_hs)
		.prop(fq::reflect::prop::Name, "ZoomMin")
		.prop(fq::reflect::prop::Comment, u8"카메라 최대 ZoomIn 정도")
		.data<&CameraMoving::mZoomMax>("ZoomMax"_hs)
		.prop(fq::reflect::prop::Name, "ZoomMax")
		.prop(fq::reflect::prop::Comment, u8"카메라 최대 ZoomOut 정도")
		.data<&CameraMoving::mZoomOutPadX>("ZoomOutPadX"_hs)
		.prop(fq::reflect::prop::Name, "ZoomOutPadX")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) ZoomOut x축 시작 위치 - 한 플레이어라도 이 위치에 있다면 ZoomOut 시작. 화면 왼쪽 끝 -1, 오른쪽 끝 1")
		.data<&CameraMoving::mZoomOutPadY>("ZoomOutPadY"_hs)
		.prop(fq::reflect::prop::Name, "ZoomOutPadY")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) ZoomOut y축 시작 위치 - 한 플레이어라도 이 위치에 있다면 ZoomOut 시작. 화면 위쪽 끝 1, 아래쪽 끝 -1. 위쪽은 좀 더 여유롭게 값을 잡아야함")
		.data<&CameraMoving::mZoomInPadX>("ZoomInPadX"_hs)
		.prop(fq::reflect::prop::Name, "ZoomInPadX")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) ZoomIn x축 시작 위치 - 한 플레이어라도 이 위치에 있다면 ZoomIn 시작. 화면 왼쪽 끝 -1, 오른쪽 끝 1")
		.data<&CameraMoving::mZoomInPadY>("ZoomInPadY"_hs)
		.prop(fq::reflect::prop::Name, "ZoomInPadY")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) ZoomIn y축 시작 위치 - 한 플레이어라도 이 위치에 있다면 ZoomIn 시작. 화면 위쪽 끝 1, 아래쪽 끝 -1. 위쪽은 좀 더 여유롭게 값을 잡아야함")
		.data<&CameraMoving::mForbiddenAreaPaddingX>("ForbiddenAreaPaddingX"_hs)
		.prop(fq::reflect::prop::Name, "ForbiddenAreaPaddingX")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) 금지영역패딩 x축 시작 위치 - ZoomOutPadX는 이 영역보다 작아야 함")
		.data<&CameraMoving::mForbiddenAreaPaddingY>("ForbiddenAreaPaddingY"_hs)
		.prop(fq::reflect::prop::Name, "ForbiddenAreaPaddingY")
		.prop(fq::reflect::prop::Comment, u8"(-1 ~ 1) 금지영역패딩 y축 시작 위치 - ZoomOutPadY는 이 영역보다 작아야 함")
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

	entt::meta<StaffSoulAttack>()
		.type("StaffSoulAttack"_hs)
		.prop(fq::reflect::prop::Name, "StaffSoulAttack")
		.data<&StaffSoulAttack::mAttackEmitTick>("AttackEmitTick"_hs)
		.prop(fq::reflect::prop::Name, "AttackEmitTick")
		.data<&StaffSoulAttack::mAttackDuration>("AttackDuration"_hs)
		.prop(fq::reflect::prop::Name, "AttackDuration")
		.data<&StaffSoulAttack::mAttack>("Attack"_hs)
		.prop(fq::reflect::prop::Name, "Attack")
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
		.data<&PauseUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&PauseUI::mRepauseUIPrefab>("ResourceUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "ResourceUIPrefab")
		.data<&PauseUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<LoadingText>()
		.type("LoadingText"_hs)
		.prop(fq::reflect::prop::POD)
		.prop(fq::reflect::prop::Name, "LoadingText")
		.data<&LoadingText::Text>("Text"_hs)
		.prop(fq::reflect::prop::Name, "Text");

	entt::meta<LoadingUI>()
		.type("LoadingUI"_hs)
		.prop(fq::reflect::prop::Name, "LoadingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&LoadingUI::mTexts>("Texts"_hs)
		.prop(fq::reflect::prop::Name, "Texts")
		.base<fq::game_module::Component>();

	entt::meta<TitleUI>()
		.type("TitleUI"_hs)
		.prop(fq::reflect::prop::Name, "TitleUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&TitleUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&TitleUI::mNextSceneName>("NextSceneName"_hs)
		.prop(fq::reflect::prop::Name, "NextSceneName")
		.data<&TitleUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<SoulSelectUI>()
		.type("SoulSelectUI"_hs)
		.prop(fq::reflect::prop::Name, "SoulSelectUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<SettingUI>()
		.type("SettingUI"_hs)
		.prop(fq::reflect::prop::Name, "SettingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<RepauseUI>()
		.type("RepauseUI"_hs)
		.prop(fq::reflect::prop::Name, "RepauseUI")
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
		.data<&MonsterGroupKill::monsterGroupName>("MonsterGroupName"_hs)
		.prop(fq::reflect::prop::Name, "MonsterGroupName");

	entt::meta<Defence>()
		.type("Defence"_hs)
		.prop(fq::reflect::prop::Name, "Defence")
		.prop(fq::reflect::prop::POD)
		.data<&Defence::colliderName>("ColliderName"_hs)
		.prop(fq::reflect::prop::Name, "ColliderName")
		.prop(reflect::prop::Comment, u8"Collider를 가진 오브젝트 이름")
		.data<&Defence::count>("Count"_hs)
		.prop(fq::reflect::prop::Name, "Count");

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

	entt::meta<RewardPortal>()
		.type("RewardPortal"_hs)
		.prop(fq::reflect::prop::Name, "RewardPortal")
		.prop(fq::reflect::prop::POD)
		.data<&RewardPortal::position>("Position"_hs)
		.prop(fq::reflect::prop::Name, "Position")
		.prop(reflect::prop::Comment, u8"포탈이 소환될 위치")
		.data<&RewardPortal::nextSceneName>("NextSceneName"_hs)
		.prop(fq::reflect::prop::Name, "NextSceneName")
		.prop(reflect::prop::Comment, u8"포탈타면 이동할 씬 이름");

	entt::meta<ArmourSpawn>()
		.type("ArmourSpawn"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSpawn")
		.prop(fq::reflect::prop::POD)
		.data<&ArmourSpawn::armourSetName>("ArmourSetName"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSetName");

	entt::meta<SequenceStart>()
		.type("SequenceStart"_hs)
		.prop(fq::reflect::prop::Name, "SequenceStart")
		.prop(fq::reflect::prop::POD)
		.data<&SequenceStart::name>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name");

	entt::meta<QuestReward>()
		.type("QuestReward"_hs)
		.prop(fq::reflect::prop::Name, "QuestReward")
		.prop(fq::reflect::prop::POD)
		.data<&QuestReward::RewardPortalList>("RewardPortalList"_hs)
		.prop(fq::reflect::prop::Name, "RewardPortalList")
		.data<&QuestReward::ArmourList>("ArmourList"_hs)
		.prop(fq::reflect::prop::Name, "ArmourList")
		.data<&QuestReward::SequenceStartList>("SequenceStartList"_hs)
		.prop(fq::reflect::prop::Name, "SequenceStartList");

	entt::meta<Quest>()
		.type("Quest"_hs)
		.prop(fq::reflect::prop::Name, "Quest")
		.prop(fq::reflect::prop::POD)
		.data<&Quest::mIndex>("Index"_hs)
		.prop(fq::reflect::prop::Name, "Index")
		.data<&Quest::mName>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name")
		.data<&Quest::mJoinConditionList>("JoinConditionList"_hs)
		.prop(fq::reflect::prop::Name, "JoinConditionList")
		.data<&Quest::mclearConditionList>("ClearConditionList"_hs)
		.prop(fq::reflect::prop::Name, "ClearConditionList")
		.data<&Quest::mRewardList>("RewardList"_hs)
		.prop(fq::reflect::prop::Name, "RewardList");

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
		.data<&QuestManager::mStartQuests>("StartQuests"_hs)
		.prop(fq::reflect::prop::Name, "StartQuests")
		.data<&QuestManager::mMainQuests>("MainQuest"_hs)
		.prop(fq::reflect::prop::Name, "MainQuest")
		.data<&QuestManager::mSubQuests>("SubQuest"_hs)
		.prop(fq::reflect::prop::Name, "SubQuest")
		.data<&QuestManager::mPortalPrefab>("PortalPrefab"_hs)
		.prop(fq::reflect::prop::Name, "PortalPrefab")
		.data<&QuestManager::mDistance>("Distance"_hs)
		.prop(fq::reflect::prop::Name, "Distance")
		.base<fq::game_module::Component>();

	entt::meta<DefenceCounter>()
		.type("DefenceCounter"_hs)
		.prop(fq::reflect::prop::Name, "DefenceCounter")
		.data<&DefenceCounter::mRequestCount>("RequestCount"_hs)
		.prop(fq::reflect::prop::Name, "RequestCount")
		.prop(reflect::prop::Comment, u8"퀘스트 클리어 요구 카운트")
		.data<&DefenceCounter::mCountSpeed>("CountSpeed"_hs)
		.prop(fq::reflect::prop::Name, "CountSpeed")
		.prop(reflect::prop::Comment, u8"게이지 차는 속도")
		.base<fq::game_module::Component>();

	entt::meta<QuestColliderTriggerChecker>()
		.type("QuestColliderTriggerChecker"_hs)
		.prop(fq::reflect::prop::Name, "QuestColliderTriggerChecker")
		.base<fq::game_module::Component>();

	entt::meta<ArmourSet>()
		.type("ArmourSet"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSet")
		.data<&ArmourSet::mArmourList>("ArmourList"_hs)
		.prop(fq::reflect::prop::Name, "ArmourList")
		.base<fq::game_module::Component>();

	entt::meta<ArmourSpawner>()
		.type("ArmourSpawner"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSpawner")
		.data<&ArmourSpawner::mArmourList>("ArmourList"_hs)
		.prop(fq::reflect::prop::Name, "ArmourList")
		.base<fq::game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                            Game Variable								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PlayerSoulVariable>()
		.type("PlayerSoulVariable"_hs)
		.prop(fq::reflect::prop::Name, "PlayerSoulVariable")
		.data<&PlayerSoulVariable::SoulBowAttackTick>("SoulBowAttackTick"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackTick")
		.data<&PlayerSoulVariable::SoulBowAttackSpeed>("SoulBowAttackSpeed"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackSpeed")
		.data<&PlayerSoulVariable::SoulBowAttackAngle>("SoulBowAttackAngle"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackAngle")
		.data<&PlayerSoulVariable::SoulBowAttackOffset>("SoulBowAttackOffset"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackOffset")
		.data<&PlayerSoulVariable::SoulBowAttackDuration>("SoulBowAttackDuration"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackDuration")

		.data<&PlayerSoulVariable::SoulAxeAttackTick>("SoulAxeAttackTick"_hs)
		.prop(fq::reflect::prop::Name, "SoulAxeAttackTick")
		.data<&PlayerSoulVariable::SoulAxeAttackDuration>("SoulAxeAttackDuration"_hs)
		.prop(fq::reflect::prop::Name, "SoulAxeAttackDuration")

		.data<&PlayerSoulVariable::SoulSwordAttackCost>("SoulSwordAttackCost"_hs)
		.prop(fq::reflect::prop::Name, "SoulSwordAttackCost")
		.data<&PlayerSoulVariable::SoulBowAttackCost>("SoulBowAttackCost"_hs)
		.prop(fq::reflect::prop::Name, "SoulBowAttackCost")
		.data<&PlayerSoulVariable::SoulAxeAttackCost>("SoulAxeAttackCost"_hs)
		.prop(fq::reflect::prop::Name, "SoulAxeAttackCost")
		.data<&PlayerSoulVariable::SoulStaffAttackCost>("SoulStaffAttackCost"_hs)
		.prop(fq::reflect::prop::Name, "SoulStaffAttackCost")

		.data<&PlayerSoulVariable::SoulGaugeCharging>("SoulGaugeCharging"_hs)
		.prop(fq::reflect::prop::Name, "SoulGaugeCharging")

		.base<IGameVariable>();

	entt::meta<DamageVariable>()
		.type("DamageVariable"_hs)
		.prop(fq::reflect::prop::Name, "DamageVariable")
		.data<&DamageVariable::SwordAttackCoefficient>("SwordAttackCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "SwordAttackCoefficient")
		.data<&DamageVariable::ShieldAttackCoefficient>("ShieldAttackCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "ShieldAttackCoefficient")
		.data<&DamageVariable::ShieldDashCoefficient>("ShieldDashCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "ShieldDashCoefficient")

		.data<&DamageVariable::MagicBallCoefficient>("MagicBallCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "MagicBallCoefficient")
		.data<&DamageVariable::MagicAOECoefficient>("MagicAOECoefficient"_hs)
		.prop(fq::reflect::prop::Name, "MagicAOECoefficient")
		.data<&DamageVariable::MagicLaserCoefficient>("MagicLaserCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "MagicLaserCoefficient")

		.data<&DamageVariable::SwordSoulCoefficient>("SwordSoulCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "SwordSoulCoefficient")
		.data<&DamageVariable::StaffSoulCoefficient>("StaffSoulCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "StaffSoulCoefficient")
		.data<&DamageVariable::AxeSoulCoefficient>("AxeSoulCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "AxeSoulCoefficient")
		.data<&DamageVariable::BowSoulCoefficient>("BowSoulCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BowSoulCoefficient")

		.data<&DamageVariable::BossSmashDownCoefficient>("BossSmashDownCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BossSmashDownCoefficient")
		.data<&DamageVariable::BossComboAttackCoefficient>("BossComboAttackCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BossComboAttackCoefficient")
		.data<&DamageVariable::BossRushCoefficient>("BossRushCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BossRushCoefficient")
		.base<IGameVariable>();

	entt::meta<SettingVariable>()
		.type("SettingVariable"_hs)
		.prop(fq::reflect::prop::Name, "SettingVariable")
		.data<&SettingVariable::MasterVolume>("MasterVolume"_hs)
		.prop(fq::reflect::prop::Name, "MasterVolume")
		.data<&SettingVariable::BGMVolume>("BGMVolume"_hs)
		.prop(fq::reflect::prop::Name, "BGMVolume")
		.data<&SettingVariable::SFXVolume>("SFXVolume"_hs)
		.prop(fq::reflect::prop::Name, "SFXVolume")

		.data<&SettingVariable::IsVibe>("IsVibe"_hs)
		.prop(fq::reflect::prop::Name, "IsVibe")
		.data<&SettingVariable::IsUsedAimAssist>("IsUsedAimAssist"_hs)
		.prop(fq::reflect::prop::Name, "IsUsedAimAssist")
		.data<&SettingVariable::ArmourSpawnDistance>("ArmourSpawnDistance"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSpawnDistance")
		.base<IGameVariable>();

	entt::meta<PlayerInfoVariable>()
		.type("PlayerInfoVariable"_hs)
		.prop(fq::reflect::prop::Name, "PlayerInfoVariable")
		.data<&PlayerInfoVariable::Player1SoulType>("Player1SoulType"_hs)
		.prop(fq::reflect::prop::Name, "Player1SoulType")
		.data<&PlayerInfoVariable::Player2SoulType>("Player2SoulType"_hs)
		.prop(fq::reflect::prop::Name, "Player2SoulType")
		.data<&PlayerInfoVariable::Player3SoulType>("Player3SoulType"_hs)
		.prop(fq::reflect::prop::Name, "Player3SoulType")
		.data<&PlayerInfoVariable::Player4SoulType>("Player4SoulType"_hs)
		.prop(fq::reflect::prop::Name, "Player4SoulType")

		.data<&PlayerInfoVariable::Player1HP>("Player1HP"_hs)
		.prop(fq::reflect::prop::Name, "Player1HP")
		.data<&PlayerInfoVariable::Player2HP>("Player2HP"_hs)
		.prop(fq::reflect::prop::Name, "Player2HP")
		.data<&PlayerInfoVariable::Player3HP>("Player3HP"_hs)
		.prop(fq::reflect::prop::Name, "Player3HP")
		.data<&PlayerInfoVariable::Player4HP>("Player4HP"_hs)
		.prop(fq::reflect::prop::Name, "Player4HP")

		.data<&PlayerInfoVariable::Player1SoulGauge>("Player1SoulGauge"_hs)
		.prop(fq::reflect::prop::Name, "Player1SoulGauge")
		.data<&PlayerInfoVariable::Player2SoulGauge>("Player2SoulGauge"_hs)
		.prop(fq::reflect::prop::Name, "Player2SoulGauge")
		.data<&PlayerInfoVariable::Player3SoulGauge>("Player3SoulGauge"_hs)
		.prop(fq::reflect::prop::Name, "Player3SoulGauge")
		.data<&PlayerInfoVariable::Player4SoulGauge>("Player4SoulGauge"_hs)
		.prop(fq::reflect::prop::Name, "Player4SoulGauge")

		.data<&PlayerInfoVariable::Player1Knight>("Player1Knight"_hs)
		.prop(fq::reflect::prop::Name, "Player1Knight")
		.data<&PlayerInfoVariable::Player2Knight>("Player2Knight"_hs)
		.prop(fq::reflect::prop::Name, "Player2Knight")
		.data<&PlayerInfoVariable::Player3Knight>("Player3Knight"_hs)
		.prop(fq::reflect::prop::Name, "Player3Knight")
		.data<&PlayerInfoVariable::Player4Knight>("Player4Knight"_hs)
		.prop(fq::reflect::prop::Name, "Player4Knight")

		.data<&PlayerInfoVariable::Player1Magic>("Player1Magic"_hs)
		.prop(fq::reflect::prop::Name, "Player1Magic")
		.data<&PlayerInfoVariable::Player2Magic>("Player2Magic"_hs)
		.prop(fq::reflect::prop::Name, "Player2Magic")
		.data<&PlayerInfoVariable::Player3Magic>("Player3Magic"_hs)
		.prop(fq::reflect::prop::Name, "Player3Magic")
		.data<&PlayerInfoVariable::Player4Magic>("Player4Magic"_hs)
		.prop(fq::reflect::prop::Name, "Player4Magic")

		.data<&PlayerInfoVariable::Player1Warrior>("Player1Warrior"_hs)
		.prop(fq::reflect::prop::Name, "Player1Warrior")
		.data<&PlayerInfoVariable::Player2Warrior>("Player2Warrior"_hs)
		.prop(fq::reflect::prop::Name, "Player2Warrior")
		.data<&PlayerInfoVariable::Player3Warrior>("Player3Warrior"_hs)
		.prop(fq::reflect::prop::Name, "Player3Warrior")
		.data<&PlayerInfoVariable::Player4Warrior>("Player4Warrior"_hs)
		.prop(fq::reflect::prop::Name, "Player4Warrior")

		.data<&PlayerInfoVariable::Player1Archer>("Player1Archer"_hs)
		.prop(fq::reflect::prop::Name, "Player1Archer")
		.data<&PlayerInfoVariable::Player2Archer>("Player2Archer"_hs)
		.prop(fq::reflect::prop::Name, "Player2Archer")
		.data<&PlayerInfoVariable::Player3Archer>("Player3Archer"_hs)
		.prop(fq::reflect::prop::Name, "Player3Archer")
		.data<&PlayerInfoVariable::Player4Archer>("Player4Archer"_hs)
		.prop(fq::reflect::prop::Name, "Player4Archer")

		.data<&PlayerInfoVariable::Player1Monster>("Player1Monster"_hs)
		.prop(fq::reflect::prop::Name, "Player1Monster")
		.data<&PlayerInfoVariable::Player2Monster>("Player2Monster"_hs)
		.prop(fq::reflect::prop::Name, "Player2Monster")
		.data<&PlayerInfoVariable::Player3Monster>("Player3Monster"_hs)
		.prop(fq::reflect::prop::Name, "Player3Monster")
		.data<&PlayerInfoVariable::Player4Monster>("Player4Monster"_hs)
		.prop(fq::reflect::prop::Name, "Player4Monster")
		.base<IGameVariable>();
	//////////////////////////////////////////////////////////////////////////
	//								  Box									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Box>()
		.type("Box"_hs)
		.prop(reflect::prop::Name, "Box")
		.prop(reflect::prop::Label, "Box")
		.data<&Box::SetmBrokenBoxPrefebPath, &Box::GetBrokenBoxPrefebPath>("BrokenBoxPrefebPath"_hs)
		.prop(reflect::prop::Name, "BrokenBoxPrefebPath")
		.prop(fq::reflect::prop::DragDrop, ".prefab")
		.prop(fq::reflect::prop::RelativePath)
		.prop(reflect::prop::Comment, u8"부서진 박스 프리펫 경로를 지정해주세요.")
		.data<&Box::SetDeadTime, &Box::GetDeadTime>("DeadTime"_hs)
		.prop(reflect::prop::Name, "DeadTime")
		.prop(reflect::prop::Comment, u8"깨지고 난 뒤 사라질 시간을 지정해주세요.")
		.base<game_module::Component>();

	//////////////////////////////////////////////////////////////////////////
	//                            Spawn Group								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<SpawnColliderTrigger>()
		.type("SpawnColliderTrigger"_hs)
		.prop(fq::reflect::prop::Name, "SpawnColliderTrigger")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnColliderTrigger::ColliderName>("ColliderName"_hs)
		.prop(fq::reflect::prop::Name, "ColliderName");

	entt::meta<EComparisonOperators>()
		.prop(fq::reflect::prop::Name, "MonsterType")
		.conv<std::underlying_type_t<EComparisonOperators>>()
		.data<EComparisonOperators::Equals>("Equals"_hs) // 0
		.prop(fq::reflect::prop::Name, "Equals")
		.data<EComparisonOperators::Greater>("Greater"_hs) // 1
		.prop(fq::reflect::prop::Name, "Greater")
		.data<EComparisonOperators::Less>("Less"_hs) // 2
		.prop(fq::reflect::prop::Name, "Less");

	entt::meta<MaxEnemy>()
		.type("MaxEnemy"_hs)
		.prop(fq::reflect::prop::Name, "MaxEnemy")
		.prop(fq::reflect::prop::POD)
		.data<&MaxEnemy::ComparisonOperator>("ComparisonOperator"_hs)
		.prop(fq::reflect::prop::Name, "ComparisonOperator")
		.data<&MaxEnemy::MonsterNum>("MonsterNum"_hs)
		.prop(fq::reflect::prop::Name, "MonsterNum");

	entt::meta<Timer>()
		.type("Timer"_hs)
		.prop(fq::reflect::prop::Name, "Timer")
		.prop(fq::reflect::prop::POD)
		.data<&Timer::Second>("Second"_hs)
		.prop(fq::reflect::prop::Name, "Second");

	entt::meta<ObjectLive>()
		.type("ObjectLive"_hs)
		.prop(fq::reflect::prop::Name, "ObjectLive")
		.prop(fq::reflect::prop::POD)
		.data<&ObjectLive::isLive>("isLive"_hs)
		.prop(fq::reflect::prop::Name, "isLive")
		.data<&ObjectLive::ObjectOrPrefabName>("ObjectOrPrefabName"_hs)
		.prop(fq::reflect::prop::Name, "ObjectOrPrefabName");

	entt::meta<SpawnCondition>()
		.type("SpawnCondition"_hs)
		.prop(fq::reflect::prop::Name, "SpawnCondition")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnCondition::SpawnColliderTriggerList>("SpawnColliderTriggerList"_hs)
		.prop(fq::reflect::prop::Name, "SpawnColliderTriggerList")
		.data<&SpawnCondition::MaxEnemyList>("MaxEnemyList"_hs)
		.prop(fq::reflect::prop::Name, "MaxEnemyList")
		.data<&SpawnCondition::TimerList>("TimerList"_hs)
		.prop(fq::reflect::prop::Name, "TimerList")
		.data<&SpawnCondition::ObjectLiveList>("ObjectLiveList"_hs)
		.prop(fq::reflect::prop::Name, "ObjectLiveList");

	entt::meta<SpawnRule>()
		.type("SpawnRule"_hs)
		.prop(fq::reflect::prop::Name, "SpawnRule")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnRule::MonsterType>("MonsterType"_hs)
		.prop(fq::reflect::prop::Name, "MonsterType")
		.data<&SpawnRule::Name>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name")
		.data<&SpawnRule::SpawnerNum>("SpawnerNum"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerNum")
		.data<&SpawnRule::SpawnMonsterNum>("SpawnMonsterNum"_hs)
		.prop(fq::reflect::prop::Name, "SpawnMonsterNum");

	entt::meta<SpawnRules>()
		.type("SpawnRules"_hs)
		.prop(fq::reflect::prop::Name, "SpawnRules")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnRules::isLoof>("isLoof"_hs)
		.prop(fq::reflect::prop::Name, "isLoof")
		.data<&SpawnRules::SpawnRuleList>("SpawnRuleList"_hs)
		.prop(fq::reflect::prop::Name, "SpawnRuleList");

	entt::meta<Spawner>()
		.type("Spawner"_hs)
		.prop(fq::reflect::prop::Name, "Spawner")
		.data<&Spawner::mID>("ID"_hs)
		.prop(fq::reflect::prop::Name, "ID")
		.base<fq::game_module::Component>();

	entt::meta<SpawnerGroup>()
		.type("SpawnerGroup"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerGroup")
		.data<&SpawnerGroup::mID>("ID"_hs)
		.prop(fq::reflect::prop::Name, "ID")
		.data<&SpawnerGroup::mStartNum>("StartNum"_hs)
		.prop(fq::reflect::prop::Name, "StartNum")
		.data<&SpawnerGroup::mSpawnConditions>("SpawnConditions"_hs)
		.prop(fq::reflect::prop::Name, "SpawnConditions")
		.data<&SpawnerGroup::mSpawnRules>("SpawnRules"_hs)
		.prop(fq::reflect::prop::Name, "SpawnRules")
		.data<&SpawnerGroup::mMeleeMonsterPrefab>("MeleeMonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "MeleeMonsterPrefab")
		.data<&SpawnerGroup::mExplosionMonsterPrefab>("ExplosionMonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "ExplosionMonsterPrefab")
		.data<&SpawnerGroup::mBossMonsterPrefab>("BossMonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterPrefab")
		.data<&SpawnerGroup::mPlantMonsterPrefab>("PlantMonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterPrefab")
		.data<&SpawnerGroup::mSpawnerMonsterPrefab>("SpawnerMonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerMonsterPrefab")
		.base<fq::game_module::Component>();
}
