#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"

// Manager
#include "GameManager.h"
#include "MaterialManager.h"
#include "MaterialManagedGroup.h"
#include "MonsterManager.h"
#include "EffectColorManager.h"
#include "EffectColorTransmitter.h"

// Player 
#include "PlayerDefine.h"
#include "Player.h"
#include "Soul.h"
#include "PlayerInputState.h"
#include "PlayerMovementState.h"
#include "PlayerMovementSoundState.h"
#include "PlayerSoundState.h"
#include "DeadArmour.h"
#include "MagicArmour.h"
#include "MagicBallAttackState.h"
#include "AOEAttackState.h"
#include "LaserAttackState.h"
#include "ShieldDashState.h"
#include "KnightArmour.h"
#include "SwordAttackState.h"
#include "ShieldBlockState.h"
#include "ShieldAttackState.h"
#include "StaffSoulAttackState.h"
#include "SwordSoulAttackState.h"
#include "BowSoulAttackState.h"
#include "AxeSoulAttackState.h"
#include "ArcherArmour.h"
#include "BowDashState.h"
#include "BowMultiShotAttackState.h"
#include "BowStrongChargingState.h"
#include "AimAssist.h"
#include "EBerserkerAttackType.h"
#include "BerserkerArmour.h"
#include "BerserkerAttackState.h"
#include "BerserkerRushState.h"
#include "BerserkerRushChargingState.h"
#include "AttackInvalidation.h"
#include "PlayerLowerMovementState.h"
#include "PlayerDummy.h"

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
#include "BossMonsterGroggyState.h"
#include "BossMonsterEatState.h"
#include "BossMonsterRoarState.h"
#include "BossMonsterContinousState.h"
#include "BossMonsterPreContinousState.h"

// PlantMoster
#include "PlantMonster.h"
#include "LinearAttack.h"
#include "PlantAOEAttack.h"
#include "DebuffPoisonZone.h"
#include "PlantMonsterAttckState.h"
#include "PlantMonsterAOEAttackState.h"
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
#include "ArrowAttack.h"
#include "KnockBack.h"

// UI
#include "HpBar.h"
#include "GaugeBar.h"
#include "PlayerUI.h"
#include "PlayerUIManager.h"
#include "BossHP.h"
#include "PauseUI.h"
#include "LoadingUI.h"
#include "TitleUI.h"
#include "SoulSelectUI.h"
#include "SettingUI.h"
#include "RepauseUI.h"
#include "ResetPopupUI.h"
#include "SavePopupUI.h"
#include "ResultUI.h"
#include "GameSettingUI.h"
#include "AudioSettingUI.h"
#include "VideoSettingUI.h"
#include "SpeechBubbleUI.h"
#include "BGaugeUI.h"
#include "PlayerCheckUI.h"

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
#include "PlayerVariable.h"
#include "SoulVariable.h"
#include "LevelVariable.h"
#include "MonsterVariable.h"

// Box
#include "Box.h"

// Suport
#include "YAxisSetter.h"

// etc
#include "BGM.h"
#include "Portal.h"
#include "GoddessStatue.h"
#include "VibrationState.h"
#include "LookingObjectHelper.h"
#include "MultiPlayerObjectCreator.h"
#include "DynamicLightHelper.h"
#include "ArmourDestroyer.h"
#include "DestroyWhenCollisionToWall.h"

void fq::client::RegisterMetaData()
{
	using namespace entt::literals;

	//////////////////////////////////////////////////////////////////////////
	//                             GameManager								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<GameManager>()
		.type("GameManager"_hs)
		.prop(reflect::prop::Name, "GameManager")
		.data<&GameManager::mIsAutoSpawn>("UseAutoSpawn"_hs)
		.prop(fq::reflect::prop::Name, "UseAutoSpawn")
		.data<&GameManager::mPauseUI>("PauseUI"_hs)
		.prop(fq::reflect::prop::Name, "PauseUI")
		.data<&GameManager::mSoul>("Soul"_hs)
		.prop(fq::reflect::prop::Name, "Soul")
		.data<&GameManager::mKnight>("Knight"_hs)
		.prop(fq::reflect::prop::Name, "Knight")
		.data<&GameManager::mMagic>("Magic"_hs)
		.prop(fq::reflect::prop::Name, "Magic")
		.data<&GameManager::mArcher>("Archer"_hs)
		.prop(fq::reflect::prop::Name, "Archer")
		.data<&GameManager::mWarrior>("Warrior"_hs)
		.prop(fq::reflect::prop::Name, "Warrior")
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
		.data<&Portal::mResultUIPrefab>("ResultUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "ResultUIPrefab")
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
		.prop(fq::reflect::prop::RelativePath)
		.data<&MaterialManager::mLocalPointOffset>("LocalPointOffset"_hs)
		.prop(fq::reflect::prop::Name, "LocalPointOffset")
		.prop(reflect::prop::Comment, u8"플레이어 반직선 로컬 오프셋")
		.data<&MaterialManager::mWorldPointOffset>("WorldPointOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldPointOffset")
		.prop(reflect::prop::Comment, u8"플레이어 반직선 월드 오프셋")
		.data<&MaterialManager::mbUseDebugDraw>("UseDebugDraw"_hs)
		.prop(fq::reflect::prop::Name, "UseDebugDraw")
		.prop(reflect::prop::Comment, u8"디버깅 정보 사용 여부")
		.data<&MaterialManager::mbUseAllStaticObjectCheck>("bUseAllStaticObjectCheck"_hs)
		.prop(fq::reflect::prop::Name, "bUseAllStaticObjectCheck")
		.prop(reflect::prop::Comment, u8"모든 스태틱 오브젝트 체크 여부")
		.data<&MaterialManager::mbUseMaterialGroup>("bUseMaterialGroup"_hs)
		.prop(fq::reflect::prop::Name, "bUseMaterialGroup")
		.prop(reflect::prop::Comment, u8"모든 머터리얼 그룹 체크 여부")
		.base<game_module::Component>();

	entt::meta<MaterialManagedGroup>()
		.type("MaterialManagedGroup"_hs)
		.prop(reflect::prop::Name, "MaterialManagedGroup")
		.data<&MaterialManagedGroup::mbUsed>("bUsed"_hs)
		.prop(fq::reflect::prop::Name, "bUsed")
		.prop(reflect::prop::Comment, u8"사용 여부")
		.base<game_module::Component>();

	entt::meta<GoddessStatue>()
		.type("GoddessStatue"_hs)
		.prop(reflect::prop::Name, "GoddessStatue")
		.data<&GoddessStatue::mDealingTime>("DealingTime"_hs)
		.prop(fq::reflect::prop::Name, "DealingTime")
		.data<&GoddessStatue::mDealingDamage>("DealingDamage"_hs)
		.prop(fq::reflect::prop::Name, "DealingDamage")
		.data<&GoddessStatue::mMaxGauge>("MaxGauge"_hs)
		.prop(fq::reflect::prop::Name, "MaxGauge")
		.data<&GoddessStatue::mFillGaugeSpeed>("FillGaugeSpeed"_hs)
		.prop(fq::reflect::prop::Name, "FillGaugeSpeed")
		.data<&GoddessStatue::mDecreaseSpeed>("DecreaseSpeed"_hs)
		.prop(fq::reflect::prop::Name, "DecreaseSpeed")
		.data<&GoddessStatue::mGodDamageBuff>("GodDamageBuff"_hs)
		.prop(fq::reflect::prop::Name, "GodDamageBuff")
		.data<&GoddessStatue::mGodCoolTimeBuff>("GodCoolTimeBuff"_hs)
		.prop(fq::reflect::prop::Name, "GodCoolTimeBuff")
		.data<&GoddessStatue::mGodMoveBuff>("GodMoveBuff"_hs)
		.prop(fq::reflect::prop::Name, "GodMoveBuff")
		.data<&GoddessStatue::mPlayerDebuff>("PlayerDebuff"_hs)
		.prop(fq::reflect::prop::Name, "PlayerDebuff")
		.data<&GoddessStatue::mPlayerBuff>("PlayerBuff"_hs)
		.prop(fq::reflect::prop::Name, "PlayerBuff")
		.data<&GoddessStatue::mBuff>("Buff"_hs)
		.prop(fq::reflect::prop::Name, "Buff")
		.base<game_module::Component>();

	entt::meta<EffectColorManager>()
		.type("EffectColorManager"_hs)
		.prop(reflect::prop::Name, "EffectColorManager")
		.data<&EffectColorManager::mKnightBaseColor>("KnightBaseColor"_hs)
		.prop(fq::reflect::prop::Name, "KnightBaseColor")
		.prop(reflect::prop::Comment, u8"검 영혼 베이스 컬러")
		.data<&EffectColorManager::mKnightEmissiveColor>("KnightEmissiveColor"_hs)
		.prop(fq::reflect::prop::Name, "KnightEmissiveColor")
		.prop(reflect::prop::Comment, u8"검 영혼 베이스 컬러")
		.data<&EffectColorManager::mBerserkerBaseColor>("BerserkerBaseColor"_hs)
		.prop(fq::reflect::prop::Name, "BerserkerBaseColor")
		.prop(reflect::prop::Comment, u8"광전사 영혼 베이스 컬러")
		.data<&EffectColorManager::mBerserkerEmissiveColor>("BerserkerEmissiveColor"_hs)
		.prop(fq::reflect::prop::Name, "BerserkerEmissiveColor")
		.prop(reflect::prop::Comment, u8"광전사 영혼 베이스 컬러")
		.data<&EffectColorManager::mArcherBaseColor>("ArcherBaseColor"_hs)
		.prop(fq::reflect::prop::Name, "ArcherBaseColor")
		.prop(reflect::prop::Comment, u8"궁수 영혼 베이스 컬러")
		.data<&EffectColorManager::mArcherEmissiveColor>("ArcherEmissiveColor"_hs)
		.prop(fq::reflect::prop::Name, "ArcherEmissiveColor")
		.prop(reflect::prop::Comment, u8"궁수 영혼 베이스 컬러")
		.data<&EffectColorManager::mMagicBaseColor>("MagicBaseColor"_hs)
		.prop(fq::reflect::prop::Name, "MagicBaseColor")
		.prop(reflect::prop::Comment, u8"마법 영혼 베이스 컬러")
		.data<&EffectColorManager::mMagicEmissiveColor>("MagicEmissiveColor"_hs)
		.prop(fq::reflect::prop::Name, "MagicEmissiveColor")
		.prop(reflect::prop::Comment, u8"마법 영혼 베이스 컬러")
		.base<game_module::Component>();

	entt::meta<EVibrationMode>()
		.type("VibrationMode"_hs)
		.prop(reflect::prop::Name, "VibrationState")
		.data<EVibrationMode::Both>("Both"_hs)
		.prop(fq::reflect::prop::Name, "Both")
		.data<EVibrationMode::Left>("Left"_hs)
		.prop(fq::reflect::prop::Name, "Left")
		.data<EVibrationMode::Right>("Right"_hs)
		.prop(fq::reflect::prop::Name, "Right");

	entt::meta<VibrationState>()
		.type("VibrationState"_hs)
		.prop(reflect::prop::Name, "VibrationState")
		.data<&VibrationState::mbUseAllController>("UseAllController"_hs)
		.prop(fq::reflect::prop::Name, "mbUseAllController")
		.prop(fq::reflect::prop::Comment, u8"모든 컨트롤러 진동 여부")
		.data<&VibrationState::mMode>("Mode"_hs)
		.prop(fq::reflect::prop::Name, "Mode")
		.prop(fq::reflect::prop::Comment, u8"진동 모드")
		.data<&VibrationState::mDuration>("Duration"_hs)
		.prop(fq::reflect::prop::Name, "Duration")
		.data<&VibrationState::mIntensity>("Intensity"_hs)
		.prop(fq::reflect::prop::Name, "Intensity")
		.prop(fq::reflect::prop::Comment, u"진동강도0 ~65535")
		.data<&VibrationState::mVibrationTime>("VibrationTime"_hs)
		.prop(fq::reflect::prop::Name, "VibrationTime")
		.prop(fq::reflect::prop::Comment, u"진동 발동 시간 ")
		.base<game_module::IStateBehaviour>();

	entt::meta<EffectColorTransmitter>()
		.type("EffectColorTransmitter"_hs)
		.prop(reflect::prop::Name, "EffectColorTransmitter")
		.data<&EffectColorTransmitter::mSoulType>("SoulType"_hs)
		.prop(fq::reflect::prop::Name, "SoulType")
		.base<game_module::Component>();

	entt::meta<LookingObjectHelper>()
		.type("LookingObjectHelper"_hs)
		.prop(reflect::prop::Name, "LookingObjectHelper")
		.data<&LookingObjectHelper::mLookingObjectName>("LookingObjectName"_hs)
		.prop(fq::reflect::prop::Name, "LookingObjectName")
		.prop(fq::reflect::prop::Comment, u"바라볼 오브젝트")
		.base<game_module::Component>();

	entt::meta<MultiPlayerObjectCreator>()
		.type("MultiPlayerObjectCreator"_hs)
		.prop(reflect::prop::Name, "MultiPlayerObjectCreator")
		.data<&MultiPlayerObjectCreator::mNum>("Num"_hs)
		.prop(fq::reflect::prop::Name, "Num")
		.data<&MultiPlayerObjectCreator::mSpawnObjectPrefab>("SpawnObject"_hs)
		.prop(fq::reflect::prop::Name, "SpawnObject")
		.base<game_module::Component>();

	entt::meta<DynamicLightHelper>()
		.type("DynamicLightHelper"_hs)
		.prop(reflect::prop::Name, "DynamicLightHelper")
		.data<&DynamicLightHelper::mMinIntensity>("MinIntensity"_hs)
		.prop(fq::reflect::prop::Name, "MinIntensity")
		.prop(fq::reflect::prop::Comment, u"최소 조명 세기")
		.data<&DynamicLightHelper::mMaxIntensity>("MaxIntensity"_hs)
		.prop(fq::reflect::prop::Name, "MaxIntensity")
		.prop(fq::reflect::prop::Comment, u"최대 조명 세기")
		.data<&DynamicLightHelper::mSpeed>("Speed"_hs)
		.prop(fq::reflect::prop::Name, "Speed")
		.prop(fq::reflect::prop::Comment, u"시간 * Speed // 기본은 1초를 기준으로 반복함")
		.base<game_module::Component>();

	entt::meta<ArmourDestroyer>()
		.type("ArmourDestroyer"_hs)
		.prop(reflect::prop::Name, "ArmourDestroyer")
		.base<game_module::Component>();

	entt::meta<DestroyWhenCollisionToWall>()
		.type("DestroyWhenCollisionToWall"_hs)
		.prop(reflect::prop::Name, "DestroyWhenCollisionToWall")
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
		.data<&Player::SetEditorHp, &Player::GetHp>("Hp"_hs)
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
		.data<&Player::mSwordHaed>("SwordHaed"_hs)
		.prop(reflect::prop::Name, "SwordHaed")
		.data<&Player::mStaffHaed>("StaffHaed"_hs)
		.prop(reflect::prop::Name, "StaffHaed")
		.data<&Player::mAxeHaed>("AxeHaed"_hs)
		.prop(reflect::prop::Name, "AxeHaed")
		.data<&Player::mBowHaed>("BowHaed"_hs)
		.prop(reflect::prop::Name, "BowHaed")

		.data<&Player::mDeadKnightArmour>("DeadKnightArmour"_hs)
		.prop(reflect::prop::Name, "DeadKnightArmour")
		.data<&Player::mDeadMagicArmour>("DeadMagicArmour"_hs)
		.prop(reflect::prop::Name, "DeadMagicArmour")
		.data<&Player::mDeadArcherArmour>("DeadArcherArmour"_hs)
		.prop(reflect::prop::Name, "DeadArcherArmour")
		.data<&Player::mDeadWarriorArmour>("DeadWarriorArmour"_hs)
		.prop(reflect::prop::Name, "DeadWarriorArmour")

		.data<&Player::mDummyPrefab>("DummyPrefab"_hs)
		.prop(reflect::prop::Name, "DummyPrefab")
		.prop(reflect::prop::Comment, u8"갑옷 해체 후 소울이 될 경우 생성되는 프리팹")

		.base<game_module::Component>();

	entt::meta<DeadArmour>()
		.type("DeadArmour"_hs)
		.prop(reflect::prop::Name, "DeadArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&DeadArmour::mLivingArmourPrefab>("LivingArmour"_hs)
		.prop(reflect::prop::Name, "LivingArmour")
		.data<&DeadArmour::mSummonDuration>("SummonDuration"_hs)
		.prop(reflect::prop::Name, "SummonDuration")
		.prop(reflect::prop::Comment, u8"갑옷 입기까지 걸리는 시간")
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
		.data<&MagicArmour::mLaserCoolTime>("LaserCoolTime"_hs)
		.prop(reflect::prop::Name, "LaserCoolTime")
		.data<&MagicArmour::mLaserCoolTimeReduction>("LaserCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "LaserCoolTimeReduction")
		.data<&MagicArmour::mAOECoolTime>("AOECoolTime"_hs)
		.prop(reflect::prop::Name, "AOECoolTime")
		.data<&MagicArmour::mAOECoolTimeReduction>("AOECoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "AOECoolTimeReduction")
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
		.data<&KnightArmour::mDashCoolTimeReduction>("DashCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "DashCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버타임 대쉬 쿨타임감소량")
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
		.data<&KnightArmour::mDashKnockBackPower>("ShieldKnockPower"_hs)
		.prop(reflect::prop::Name, "ShieldKnockPower")
		.prop(reflect::prop::Comment, u8"대쉬 넉백")
		.data<&KnightArmour::mShieldSpeedRatio>("ShieldSpeedRatio"_hs)
		.prop(reflect::prop::Name, "ShieldSpeedRatio")
		.prop(reflect::prop::Comment, u8"쉴드 상태 이동속도 배율")
		.data<&KnightArmour::mShieldCoolTime>("ShieldCoolTime"_hs)
		.prop(reflect::prop::Name, "ShieldCoolTime")
		.prop(reflect::prop::Comment, u8"쉴드 쿨타임")
		.data<&KnightArmour::mShieldCoolTimeReduction>("ShieldCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "ShieldCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버타임 쉴드 쿨타임감소량")
		.data<&KnightArmour::mShieldDuration>("ShieldDuration"_hs)
		.prop(reflect::prop::Name, "ShieldDuration")
		.prop(reflect::prop::Comment, u8"쉴드 지속시간")
		.data<&KnightArmour::mShieldKnockBackPower>("ShieldKnockBackPower"_hs)
		.prop(reflect::prop::Name, "ShieldKnockBackPower")
		.prop(reflect::prop::Comment, u8"쉴드 넉백")
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
		.data<&KnightArmour::mShieldCollider>("ShieldCollider"_hs)
		.prop(reflect::prop::Name, "ShieldCollider")
		.base<game_module::Component>();

	entt::meta<ArcherArmour>()
		.type("ArcherArmour"_hs)
		.prop(reflect::prop::Name, "ArcherArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&ArcherArmour::mDashCoolTime>("DashCoolTime"_hs)
		.prop(reflect::prop::Name, "DashCoolTime")
		.prop(reflect::prop::Comment, u8"구르기 쿨타임")
		.data<&ArcherArmour::mDashCoolTimeReduction>("DashCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "DashCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버시 구르기 쿨타임 감소량")
		.data<&ArcherArmour::mStrongAttackCoolTime>("StrongAttackCoolTime"_hs)
		.prop(reflect::prop::Name, "StrongAttackCoolTime")
		.prop(reflect::prop::Comment, u8"X공격(강공격) 쿨타임")
		.data<&ArcherArmour::mStrongAttackCoolTimeReduction>("StrongAttackCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "StrongAttackCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버시 X공격(강공격) 쿨타임 감소량")
		.data<&ArcherArmour::mWeakProjectileVelocity>("WeakProjectileVelocity"_hs)
		.prop(reflect::prop::Name, "WeakProjectileVelocity")
		.prop(reflect::prop::Comment, u8"약공격 속도")
		.data<&ArcherArmour::mStrongProjectileVelocity>("StrongProjectileVelocity"_hs)
		.prop(reflect::prop::Name, "StrongProjectileVelocity")
		.prop(reflect::prop::Comment, u8"강공격 속도")
		.data<&ArcherArmour::mWeakAttack>("mWeakAttack"_hs)
		.prop(reflect::prop::Name, "mWeakAttack")
		.prop(reflect::prop::Comment, u8"약공격 프리펩")
		.data<&ArcherArmour::mStrongAttack>("mStrongAttack"_hs)
		.prop(reflect::prop::Name, "mStrongAttack")
		.prop(reflect::prop::Comment, u8"강공격 프리펩")
		.data<&ArcherArmour::mStrongAttackChargingEffect>("mStrongAttackChargingEffect"_hs)
		.prop(reflect::prop::Name, "mStrongAttackChargingEffect")
		.prop(reflect::prop::Comment, u8"차징 이펙트")
		.data<&ArcherArmour::mStrongAttackLaunchEffect>("mStrongAttackLaunchEffect"_hs)
		.prop(reflect::prop::Name, "mStrongAttackLaunchEffect")
		.prop(reflect::prop::Comment, u8"강공격 이펙트 프리펩")
		.data<&ArcherArmour::mDashEffect>("mDashEffect"_hs)
		.prop(reflect::prop::Name, "mDashEffect")
		.prop(reflect::prop::Comment, u8"구르기 이펙트")
		.base<game_module::Component>();

	entt::meta<BerserkerArmour>()
		.type("BerserkerArmour"_hs)
		.prop(reflect::prop::Name, "BerserkerArmour")
		.prop(reflect::prop::Label, "Player")
		.data<&BerserkerArmour::mBoxAttackPrefab>("BoxAttackPrefab"_hs)
		.prop(reflect::prop::Name, "BoxAttackPrefab")
		.prop(reflect::prop::Comment, u8"공격 시 생성되는 기본 콜라이더(박스형)")
		.data<&BerserkerArmour::mCircleAttackPrefab>("CircleAttackPrefab"_hs)
		.prop(reflect::prop::Name, "CircleAttackPrefab")
		.prop(reflect::prop::Comment, u8"공격 시 생성되는 기본 콜라이더(원형)")

		.data<&BerserkerArmour::mLeftAttackSound>("LeftAttackSound"_hs)
		.prop(reflect::prop::Name, "LeftAttackSound")
		.prop(reflect::prop::Comment, u8"기본공격(1타) 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mRightAttackSound>("RightAttackSound"_hs)
		.prop(reflect::prop::Name, "RightAttackSound")
		.prop(reflect::prop::Comment, u8"기본공격(2타) 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mStrikeDownAttackSound>("StrikeDownAttackSound"_hs)
		.prop(reflect::prop::Name, "StrikeDownAttackSound")
		.prop(reflect::prop::Comment, u8"기본공격(3타) 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mSwingAroundSound>("SwingAroundSound"_hs)
		.prop(reflect::prop::Name, "SwingAroundSound")
		.prop(reflect::prop::Comment, u8"휩쓸기 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mAttackRushSound>("AttackRushSound"_hs)
		.prop(reflect::prop::Name, "AttackRushSound")
		.prop(reflect::prop::Comment, u8"돌진 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mAttackRushReadySound>("AttackRushReadySound"_hs)
		.prop(reflect::prop::Name, "AttackRushReadySound")
		.prop(reflect::prop::Comment, u8"돌진 사운드(사운드 클립키)")

		.data<&BerserkerArmour::mLeftAttackHitSound>("LeftAttackHitSound"_hs)
		.prop(reflect::prop::Name, "LeftAttackHitSound")
		.prop(reflect::prop::Comment, u8"기본공격(1타)로 피해 입을 시 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mRightAttackHitSound>("RightAttackHitSound"_hs)
		.prop(reflect::prop::Name, "RightAttackHitSound")
		.prop(reflect::prop::Comment, u8"기본공격(2타)로 피해 입을 시 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mStrikeDownAttackHitSound>("StrikeDownAttackHitSound"_hs)
		.prop(reflect::prop::Name, "StrikeDownAttackHitSound")
		.prop(reflect::prop::Comment, u8"기본공격(3타)로 피해 입을 시 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mSwingAroundHitSound>("SwingAroundHitSound"_hs)
		.prop(reflect::prop::Name, "SwingAroundHitSound")
		.prop(reflect::prop::Comment, u8"휩쓸기로 인해 피해 입을 시 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mAttackRushHitSound>("RushHitSound"_hs)
		.prop(reflect::prop::Name, "RushHitSound")
		.prop(reflect::prop::Comment, u8"돌진으로 인해 피해 입을 시 사운드(사운드 클립키)")
		.data<&BerserkerArmour::mSwingAroundCoolTime>("mSwingAroundCoolTime"_hs)
		.prop(reflect::prop::Name, "mSwingAroundCoolTime")
		.prop(reflect::prop::Comment, u8"휩쓸기(A 버튼) 쿨타임")
		.data<&BerserkerArmour::mSwingAroundCoolTimeReduction>("SwingAroundCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "SwingAroundCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버시 휩쓸기(A 버튼) 쿨타임 감소량")
		.data<&BerserkerArmour::mRushCoolTime>("RushCoolTime"_hs)
		.prop(reflect::prop::Name, "RushCoolTime")
		.prop(reflect::prop::Comment, u8"돌진(R 스틱) 쿨타임")
		.data<&BerserkerArmour::mRushCoolTimeReduction>("RushCoolTimeReduction"_hs)
		.prop(reflect::prop::Name, "RushCoolTimeReduction")
		.prop(reflect::prop::Comment, u8"피버시 돌진(R 스틱) 쿨타임 감소량")
		.data<&BerserkerArmour::mTargetPosRatio>("Pushed Back Ratio"_hs)
		.prop(reflect::prop::Name, "Pushed Back Ratio")
		.prop(reflect::prop::Comment, u8"기본 공격 시 뒤로 밀리는 비율")
		.data<&BerserkerArmour::mDirectionRatio>("Pushed Attack Direction Ratio"_hs)
		.prop(reflect::prop::Name, "Pushed Attack Direction Ratio")
		.prop(reflect::prop::Comment, u8"기본 공격 시 공격 방향으로 밀리는 비율")
		.base<game_module::Component>();

	entt::meta<Soul>()
		.type("Soul"_hs)
		.prop(reflect::prop::Name, "Soul")
		.prop(reflect::prop::Label, "Player")
		.data<&Soul::SetSoulType, &Soul::GetSoulType>("SoulType"_hs)
		.prop(reflect::prop::Name, "SoulType")
		.base<game_module::Component>();

	entt::meta<AimAssist>()
		.type("AimAssist"_hs)
		.prop(reflect::prop::Name, "AimAssist")
		.prop(reflect::prop::Label, "Player")
		.data<&AimAssist::mTheta>("Theta"_hs)
		.prop(reflect::prop::Name, "Theta")
		.prop(reflect::prop::Comment, u8"보정할 각도, 기본 180도")
		.base<game_module::Component>();

	entt::meta<PlayerDummy>()
		.type("PlayerDummy"_hs)
		.prop(reflect::prop::Name, "PlayerDummy")
		.prop(reflect::prop::Label, "Player")
		.data<&PlayerDummy::mDuration>("Duration"_hs)
		.prop(reflect::prop::Name, "Duration")
		.prop(reflect::prop::Comment, u8"더미 오브젝트 생존 시간")
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

	entt::meta<PlayerLowerMovementState>()
		.type("PlayerLowerMovementState"_hs)
		.prop(reflect::prop::Name, "PlayerLowerMovementState")
		.data<&PlayerLowerMovementState::mbOnEnterToLowerMovement>("OnEnterToLowerMovement"_hs)
		.prop(reflect::prop::Name, "OnEnterToLowerMovement")
		.data<&PlayerLowerMovementState::mbOffExitToLowerMovement>("OffExitToLowerMovement"_hs)
		.prop(reflect::prop::Name, "OffExitToLowerMovement")
		.base<game_module::IStateBehaviour>();


	entt::meta<PlayerMovementSoundState>()
		.type("PlayerMovementSoundState"_hs)
		.prop(reflect::prop::Name, "PlayerMovementSoundState")
		.data<&PlayerMovementSoundState::mWalkSoundTurm>("WalkSoundTurm"_hs)
		.prop(reflect::prop::Name, "WalkSoundTurm")
		.base<game_module::IStateBehaviour>();

	entt::meta<PlayerSoundState>()
		.type("PlayerSoundState"_hs)
		.prop(reflect::prop::Name, "PlayerSoundState")
		.data<&PlayerSoundState::mbIsPlayLoop>("IsPlayLoop"_hs)
		.prop(reflect::prop::Name, "IsPlayLoop")
		.prop(reflect::prop::Comment, u8"반복 재생 여부")
		.data<&PlayerSoundState::mbIsPlayStateEnter>("IsPlayStateEnter"_hs)
		.prop(reflect::prop::Name, "IsPlayStateEnter")
		.prop(reflect::prop::Comment, u8"상태 진입 시 사운드 재생 여부")
		.data<&PlayerSoundState::mSoundTurm>("SoundTurm"_hs)
		.prop(reflect::prop::Name, "SoundTurm")
		.prop(reflect::prop::Comment, u8"반복 재생 시간")
		.data<&PlayerSoundState::mbUseRandomPlay>("UseRandomPlay"_hs)
		.prop(reflect::prop::Name, "UseRandomPlay")
		.prop(reflect::prop::Comment, u8"랜덤 재생 여부, off 시 순차적으로 사운드 재생")
		.data<&PlayerSoundState::mSoundNames>("mSoundNames"_hs)
		.prop(reflect::prop::Name, "mSoundNames")
		.prop(reflect::prop::Comment, u8"상태 진입 시 사운드 재생 여부")
		.base<game_module::IStateBehaviour>();

	entt::meta<MagicBallAttackState>()
		.type("MagicAttackState"_hs)
		.prop(reflect::prop::Name, "MagicAttackState")
		.data<&MagicBallAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.prop(reflect::prop::Comment, u8"공격 시간")
		.data<&MagicBallAttackState::mCheckInputTime>("CheckInputTime"_hs)
		.prop(reflect::prop::Name, "CheckInputTime")
		.prop(reflect::prop::Comment, u8"Hold 입력을 받는 시간")
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

	entt::meta<ShieldBlockState>()
		.type("ShieldBlockState"_hs)
		.prop(reflect::prop::Name, "ShieldBlockState")
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

	entt::meta<BowDashState>()
		.type("BowDashState"_hs)
		.prop(reflect::prop::Name, "BowDashState")
		.data<&BowDashState::mMaxSpeedMultiplier>("MaxSpeed"_hs)
		.prop(reflect::prop::Name, "MaxSpeed")
		.prop(fq::reflect::prop::Comment, u8"최대로 늘어날 스피드 배수")
		.data<&BowDashState::mMinSpeedMultiplier>("MinSpeed"_hs)
		.prop(reflect::prop::Name, "MinSpeed")
		.prop(fq::reflect::prop::Comment, u8"최소 스피드 배수")
		.data<&BowDashState::mRotationSpeed>("RotationSpeed"_hs)
		.prop(reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"회전 스피드")
		.data<&BowDashState::mPeakSpeedTime>("PeakSpeedTime"_hs)
		.prop(reflect::prop::Name, "PeakSpeedTime")
		.prop(fq::reflect::prop::Comment, u8"최고 스피드가 도달하는 시간")
		.data<&BowDashState::mSpeedDecayRate>("SpeedDecayRate"_hs)
		.prop(reflect::prop::Name, "SpeedDecayRate")
		.prop(fq::reflect::prop::Comment, u8"최고 스피드 이후로 초당 줄어드는 속도값")
		.base<game_module::IStateBehaviour>();

	entt::meta<BowMultiShotAttackState>()
		.type("BowMultiShotAttackState"_hs)
		.prop(reflect::prop::Name, "BowMultiShotAttackState")
		.data<&BowMultiShotAttackState::mShotDelay>("ShotDelay"_hs)
		.prop(reflect::prop::Name, "ShotDelay")
		.prop(fq::reflect::prop::Comment, u8"화살 연사 속도")
		.base<game_module::IStateBehaviour>();

	entt::meta<BowStrongChargingState>()
		.type("BowStrongChargingState"_hs)
		.prop(reflect::prop::Name, "BowStrongChargingState")
		.data<&BowStrongChargingState::mRotationSpeed>("RotationSpeed"_hs)
		.prop(reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"회전 속도")
		.data<&BowStrongChargingState::mForcedChargingWaitingTime>("ForcedChargingWaitingTime"_hs)
		.prop(reflect::prop::Name, "ForcedChargingWaitingTime")
		.prop(fq::reflect::prop::Comment, u8"강제 차징 대기 시간")
		.data<&BowStrongChargingState::mChargingWationTime>("ChargingWationTime"_hs)
		.prop(reflect::prop::Name, "ChargingWationTime")
		.prop(fq::reflect::prop::Comment, u8"다음 단계로 가기 위한 대기 시간")
		.data<&BowStrongChargingState::mStringAttackIndex>("StringAttackIndex"_hs)
		.prop(reflect::prop::Name, "StringAttackIndex")
		.prop(fq::reflect::prop::Comment, u8"강공격 단계 (1 ~ 4)")
		.base<game_module::IStateBehaviour>();

	entt::meta<EBerserkerAttackType>()
		.prop(fq::reflect::prop::Name, "ArmourType")
		.conv<std::underlying_type_t<EBerserkerAttackType>>()
		.data<EBerserkerAttackType::Left>("Left"_hs) // 0
		.prop(fq::reflect::prop::Name, "Left")
		.data<EBerserkerAttackType::Right>("Right"_hs) // 1
		.prop(fq::reflect::prop::Name, "Right")
		.data<EBerserkerAttackType::StrikeDown>("StrikeDown"_hs) // 2
		.prop(fq::reflect::prop::Name, "StrikeDown")
		.data<EBerserkerAttackType::SwingAround>("SwingAround"_hs) // 3
		.prop(fq::reflect::prop::Name, "SwingAround")
		.data<EBerserkerAttackType::Rush>("Rush"_hs) // 4
		.prop(fq::reflect::prop::Name, "Rush")
		.data<EBerserkerAttackType::None>("None"_hs) // 5
		.prop(fq::reflect::prop::Name, "None");

	entt::meta<BerserkerAttackState>()
		.type("BerserkerAttackState"_hs)
		.prop(reflect::prop::Name, "BerserkerAttackState")
		.data<&BerserkerAttackState::mAttackType>("AttackType"_hs)
		.prop(reflect::prop::Name, "AttackType")
		.prop(fq::reflect::prop::Comment, u8"공격 종류")
		.data<&BerserkerAttackState::mAttackMovement>("AttackMovement"_hs)
		.prop(reflect::prop::Name, "AttackMovement")
		.prop(fq::reflect::prop::Comment, u8"공격 시 이동될 움직임")
		.data<&BerserkerAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 생성 시기")
		.data<&BerserkerAttackState::mColliderOffset>("ColliderOffset"_hs)
		.prop(reflect::prop::Name, "ColliderOffset")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 오프셋")
		.data<&BerserkerAttackState::mColliderScale>("ColliderScale"_hs)
		.prop(reflect::prop::Name, "ColliderScale")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 스케일")
		.data<&BerserkerAttackState::mKnocBackPower>("KnocBackPower"_hs)
		.prop(reflect::prop::Name, "KnocBackPower")
		.prop(fq::reflect::prop::Comment, u8"피격된 물체가 밀리는 정도")
		.data<&BerserkerAttackState::mDestroyTime>("DestroyTime"_hs)
		.prop(reflect::prop::Name, "DestroyTime")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 지속 시간")
		.base<game_module::IStateBehaviour>();

	entt::meta<BerserkerRushState>()
		.type("BerserkerRushState"_hs)
		.prop(reflect::prop::Name, "BerserkerRushState")
		.data<&BerserkerRushState::mAttackType>("AttackType"_hs)
		.prop(reflect::prop::Name, "AttackType")
		.prop(fq::reflect::prop::Comment, u8"공격 종류")
		.data<&BerserkerRushState::mAttackMovement>("AttackMovement"_hs)
		.prop(reflect::prop::Name, "AttackMovement")
		.prop(fq::reflect::prop::Comment, u8"공격 시 이동될 움직임")
		.data<&BerserkerRushState::mAttackTiming>("AttackTiming"_hs)
		.prop(reflect::prop::Name, "AttackTiming")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 생성 시기")
		.data<&BerserkerRushState::mColliderOffset>("ColliderOffset"_hs)
		.prop(reflect::prop::Name, "ColliderOffset")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 오프셋")
		.data<&BerserkerRushState::mColliderScale>("ColliderScale"_hs)
		.prop(reflect::prop::Name, "ColliderScale")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 스케일")
		.data<&BerserkerRushState::mKnocBackPower>("KnocBackPower"_hs)
		.prop(reflect::prop::Name, "KnocBackPower")
		.prop(fq::reflect::prop::Comment, u8"피격된 물체가 밀리는 정도")
		.data<&BerserkerRushState::mDestroyTime>("DestroyTime"_hs)
		.prop(reflect::prop::Name, "DestroyTime")
		.prop(fq::reflect::prop::Comment, u8"공격 콜라이더 지속 시간")
		.data<&BerserkerRushState::mVelocity>("Velocity"_hs)
		.prop(reflect::prop::Name, "Velocity")
		.prop(fq::reflect::prop::Comment, u8"돌진 속도")
		.data<&BerserkerRushState::mAcceleration>("Acceleration"_hs)
		.prop(reflect::prop::Name, "Acceleration")
		.prop(fq::reflect::prop::Comment, u8"돌진 가속도")
		.data<&BerserkerRushState::mMaxVelocity>("MaxVelocity"_hs)
		.prop(reflect::prop::Name, "MaxVelocity")
		.prop(fq::reflect::prop::Comment, u8"돌진 최대 속도 max(속 + 가속 * 시간, MaxVelocity)")
		.base<game_module::IStateBehaviour>();

	entt::meta<BerserkerRushChargingState>()
		.type("BerserkerRushChargingState"_hs)
		.prop(reflect::prop::Name, "BerserkerRushChargingState")
		.data<&BerserkerRushChargingState::mChargingTime>("ChargingTime"_hs)
		.prop(reflect::prop::Name, "ChargingTime")
		.prop(fq::reflect::prop::Comment, u8"차징 대기 시간")
		.data<&BerserkerRushChargingState::mChargingMinimumTime>("ChargingMinimumTime"_hs)
		.prop(reflect::prop::Name, "ChargingMinimumTime")
		.prop(fq::reflect::prop::Comment, u8"Rush 상태 진입을 위한 최소 차징 시간")
		.data<&BerserkerRushChargingState::mRotationSpeed>("RotationSpeed"_hs)
		.prop(reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"회진 속도, 현재방향에서 패드 방향까지 전환에 적용될 시간, 소요시간 : 1 / speed")
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
		.data<&MeleeMonster::mDummyTraceDurationTime>("DummyTraceDurationTime"_hs)
		.prop(fq::reflect::prop::Name, "DummyTraceDurationTime")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 추적 시간")
		.data<&MeleeMonster::mbUseDummyTraceRandomRange>("bUseDummyTraceRandomRange"_hs)
		.prop(fq::reflect::prop::Name, "bUseDummyTraceRandomRange")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 추적 시간에 더해질 랜덤 추적 시간을 사용할지 여부")
		.data<&MeleeMonster::mDummyDurationRandomRangeMin>("DummyDurationRandomRangeMin"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMin")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최소 랜덤 추적 시간(최소값 ~ 최댓값)")
		.data<&MeleeMonster::mDummyDurationRandomRangeMax>("DummyDurationRandomRangeMax"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMax")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최대 랜덤 추적 시간(최소값 ~ 최댓값)")
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
		.data<&MeleeMonsterDeadState::mEraseTime>("EraseTime"_hs)
		.prop(fq::reflect::prop::Name, "EraseTime")
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
		.data<&BossMonster::mRushKnockBackPower>("RushKnockBackPower"_hs)
		.prop(fq::reflect::prop::Name, "RushKnockBackPower")
		.data<&BossMonster::mContinousKnockBackPower>("ContinousKnockBackPower"_hs)
		.prop(fq::reflect::prop::Name, "ContinousKnockBackPower")
		.data<&BossMonster::mComboAttackKnockBackPower>("ComboAttackKnockBackPower"_hs)
		.prop(fq::reflect::prop::Name, "ComboAttackKnockBackPower")
		.data<&BossMonster::mSmashKnockBackPower>("SmashKnockBackPower"_hs)
		.prop(fq::reflect::prop::Name, "SmashKnockBackPower")
		.data<&BossMonster::mComboAttackReboundPower>("ComboAttackReboundPower"_hs)
		.prop(fq::reflect::prop::Name, "ComboAttackReboundPower")
		.data<&BossMonster::mAttackRange>("AttackRange"_hs)
		.prop(fq::reflect::prop::Name, "AttackRange")
		.data<&BossMonster::mDetectRange>("DetectRange"_hs)
		.prop(fq::reflect::prop::Name, "DetectRange")
		.prop(fq::reflect::prop::Comment, u8"플레이어 감지 범위")
		.data<&BossMonster::mRotationSpeed>("RotationSpeed"_hs)
		.prop(fq::reflect::prop::Name, "RotationSpeed")
		.data<&BossMonster::mSecondComboAttackRatio>("SecondComboAttackRatio"_hs)
		.prop(fq::reflect::prop::Name, "SecondComboAttackRatio")
		.prop(fq::reflect::prop::Comment, u8"콤보 공격에서 2타만 사용하는 확률 0 ~ 1")
		.data<&BossMonster::mMinWaitAttackTime>("MinWaitAttackTime"_hs)
		.prop(fq::reflect::prop::Name, "minWaitAttackTime")
		.prop(fq::reflect::prop::Comment, u8"공격하고 대기하는 최소 시간")
		.data<&BossMonster::mMaxWaitAttackTime>("MaxWaitAttackTime"_hs)
		.prop(fq::reflect::prop::Name, "MaxWaitAttackTime")
		.prop(fq::reflect::prop::Comment, u8"공격하고 대기하는 최대 시간")
		.data<&BossMonster::mGroggyIncreaseRatio>("GroggyIncreaseRatio"_hs)
		.prop(fq::reflect::prop::Name, "GroggyIncreaseRatio")
		.prop(fq::reflect::prop::Comment, u8"피격시 대미지 비례 그로기 게이지 증가량")
		.data<&BossMonster::mGroggyDecreasePerSecond>("GroggyDecreasePerSecond"_hs)
		.prop(fq::reflect::prop::Name, "GroggyDecreasePerSecond")
		.prop(fq::reflect::prop::Comment, u8"초당 그로기 게이지 감소량")

		.data<&BossMonster::mRushProbability>("RushProbability"_hs)
		.prop(fq::reflect::prop::Name, "RushProbability")
		.prop(fq::reflect::prop::Comment, u8"러쉬 패턴 확률\n확률합계는 1.f 이하이고 남은 확률은 콤보 공격 확률이 됩니다")
		.data<&BossMonster::mSmashProbability>("SmashProbability"_hs)
		.prop(fq::reflect::prop::Name, "SmashProbability")
		.prop(fq::reflect::prop::Comment, u8"내려찍기 패턴 확률 ")
		.data<&BossMonster::mRoarProbability>("RoarProbability"_hs)
		.prop(fq::reflect::prop::Name, "RoarProbability")
		.prop(fq::reflect::prop::Comment, u8"로어 패턴 확률")
		.data<&BossMonster::mContinousProbability>("ContinousProbability"_hs)
		.prop(fq::reflect::prop::Name, "ContinousProbability")
		.prop(fq::reflect::prop::Comment, u8"연속 공격 패턴 확률")
		.data<&BossMonster::mEatProbability>("EatProbability"_hs)
		.prop(fq::reflect::prop::Name, "EatProbability")
		.prop(fq::reflect::prop::Comment, u8"먹기 패턴 확률")
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

		.data<&BossMonster::mDummyTraceDurationTime>("DummyTraceDurationTime"_hs)
		.prop(fq::reflect::prop::Name, "DummyTraceDurationTime")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 추적 시간")
		.data<&BossMonster::mbUseDummyTraceRandomRange>("bUseDummyTraceRandomRange"_hs)
		.prop(fq::reflect::prop::Name, "bUseDummyTraceRandomRange")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 추적 시간에 더해질 랜덤 추적 시간을 사용할지 여부")
		.data<&BossMonster::mDummyDurationRandomRangeMin>("DummyDurationRandomRangeMin"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMin")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최소 랜덤 추적 시간(최소값 ~ 최댓값)")
		.data<&BossMonster::mDummyDurationRandomRangeMax>("DummyDurationRandomRangeMax"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMax")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최대 랜덤 추적 시간(최소값 ~ 최댓값)")

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
		.data<&BossMonsterComboAttackState::mXAxisOffset>("XAxisOffset"_hs)
		.prop(fq::reflect::prop::Name, "XAxisOffset")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterPrepareAttackState>()
		.type("BossMonsterPrepareAttackState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterPrepareAttackState")
		.data<&BossMonsterPrepareAttackState::mHomingTime>("HomingTime"_hs)
		.prop(fq::reflect::prop::Name, "HomingTime")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterGroggyState>()
		.type("BossMonsterGroggyState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterGroggyState")
		.data<&BossMonsterGroggyState::mGroggyTime>("GroggyTime"_hs)
		.prop(fq::reflect::prop::Name, "GroggyTime")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterEatState>()
		.type("BossMonsterEatState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterEatState")
		.data<&BossMonsterEatState::mEatTime>("EatTime"_hs)
		.prop(fq::reflect::prop::Name, "EatTime")
		.data<&BossMonsterEatState::mRecoverHp>("RecoverHp"_hs)
		.prop(fq::reflect::prop::Name, "RecoverHp")
		.prop(fq::reflect::prop::Comment, u8"초당 HP 회복량")
		.data<&BossMonsterEatState::mRimLightColor>("RimLightColor"_hs)
		.prop(fq::reflect::prop::Name, "RimLightColor")
		.prop(fq::reflect::prop::Comment, u8"체력회복시 림라이트 색깔")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterRoarState>()
		.type("BossMonsterRoarState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterRoarState")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterContinousState>()
		.type("BossMonsterContinousState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterContinousState")
		.data<&BossMonsterContinousState::mAttackDuration>("AttackDuration"_hs)
		.prop(fq::reflect::prop::Name, "AttackDuration")
		.prop(fq::reflect::prop::Comment, u8"공격 지속시간")
		.base<fq::game_module::IStateBehaviour>();

	entt::meta<BossMonsterPreContinousState>()
		.type("BossMonsterPreContinousState"_hs)
		.prop(fq::reflect::prop::Name, "BossMonsterPreContinousState")
		.base<fq::game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             원거리 몬스터 	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<PlantMonster>()
		.type("PlantMonster"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonster")
		.prop(reflect::prop::Label, "Monster")
		.data<&PlantMonster::mbIsAOEAttacker>("IsAOEAttacker"_hs)
		.prop(fq::reflect::prop::Name, "IsAOEAttacker")
		.prop(fq::reflect::prop::Comment, u8"범위 공격 하는 원거리 몬스터인가?")
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
		.data<&PlantMonster::mAOEAttackPrefab>("AOEAttackPrefab"_hs)
		.prop(fq::reflect::prop::Name, "AOEAttackPrefab")
		.data<&PlantMonster::mAttackCoolTime>("AttackCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackCoolTime")
		.data<&PlantMonster::mRotationSpeed>("RotationSpeed"_hs)
		.prop(fq::reflect::prop::Name, "RotationSpeed")
		.prop(fq::reflect::prop::Comment, u8"회전 속도 0 < x <= 1.f")

		.data<&PlantMonster::mDummyTraceDurationTime>("DummyTraceDurationTime"_hs)
		.prop(fq::reflect::prop::Name, "DummyTraceDurationTime")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 추적 시간")
		.data<&PlantMonster::mbUseDummyTraceRandomRange>("bUseDummyTraceRandomRange"_hs)
		.prop(fq::reflect::prop::Name, "bUseDummyTraceRandomRange")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 추적 시간에 더해질 랜덤 추적 시간을 사용할지 여부")
		.data<&PlantMonster::mDummyDurationRandomRangeMin>("DummyDurationRandomRangeMin"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMin")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최소 랜덤 추적 시간(최소값 ~ 최댓값)")
		.data<&PlantMonster::mDummyDurationRandomRangeMax>("DummyDurationRandomRangeMax"_hs)
		.prop(fq::reflect::prop::Name, "DummyDurationRandomRangeMax")
		.prop(fq::reflect::prop::Comment, u8"더미 플레이어 생성 시 더해질 최대 랜덤 추적 시간(최소값 ~ 최댓값)")

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

	entt::meta<PlantMonsterAOEAttackState>()
		.type("PlantMonsterAOEAttackState"_hs)
		.prop(fq::reflect::prop::Name, "PlantMonsterAOEAttackState")
		.data<&PlantMonsterAOEAttackState::mAttackTiming>("AttackTiming"_hs)
		.prop(fq::reflect::prop::Name, "AttackTiming")
		.data<&PlantMonsterAOEAttackState::mLookAtTime>("LookAtTime"_hs)
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
		.data<&PlantMonsterDeadState::mEraseTime>("EraseTime"_hs)
		.prop(fq::reflect::prop::Name, "EraseTime")
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
		.data<&Attack::mAttackEffectEvent>("HitEffect"_hs)
		.prop(fq::reflect::prop::Name, "HitEffect")
		.prop(fq::reflect::prop::Comment, u8"해당 공격에 피격 시 발생할 StateEvent 이펙트 이름")
		.base<fq::game_module::Component>();

	entt::meta<LinearAttack>()
		.type("LinearAttack"_hs)
		.prop(fq::reflect::prop::Name, "LinearAttack")
		.base<fq::game_module::Component>();

	entt::meta<PlantAOEAttack>()
		.type("PlantAOEAttack"_hs)
		.prop(fq::reflect::prop::Name, "PlantAOEAttack")
		.data<&PlantAOEAttack::mDestroyTime>("DestroyTime"_hs)
		.prop(fq::reflect::prop::Name, "DestroyTime")
		.prop(fq::reflect::prop::Comment, u8"범위 공격이 없어지는 시간")
		.data<&PlantAOEAttack::mMinArrivalTime>("MinArrivalTime"_hs)
		.prop(fq::reflect::prop::Name, "MinArrivalTime")
		.prop(fq::reflect::prop::Comment, u8"최소 도착 시간")
		.data<&PlantAOEAttack::mMaxArrivalTime>("MaxArrivalTime"_hs)
		.prop(fq::reflect::prop::Name, "MaxArrivalTime")
		.prop(fq::reflect::prop::Comment, u8"최대 도착 시간")
		.data<&PlantAOEAttack::mPoisonDamage>("PoisonDamage"_hs)
		.prop(fq::reflect::prop::Name, "PoisonDamage")
		.prop(fq::reflect::prop::Comment, u8"독 데미지")
		.data<&PlantAOEAttack::mPoisonTurm>("PoisonTurm"_hs)
		.prop(fq::reflect::prop::Name, "PoisonTurm")
		.prop(fq::reflect::prop::Comment, u8"독 데미지 받는 시간 딜레이")
		.data<&PlantAOEAttack::mAOEAttackSplashEffectPrefeb>("AOEAttackSplashEffectPrefeb"_hs)
		.prop(fq::reflect::prop::Name, "AOEAttackSplashEffectPrefeb")
		.prop(fq::reflect::prop::Comment, u8"스플래쉬 이펙트 프리펩 경로")
		.data<&PlantAOEAttack::mAOEAttackPoolEffectPrefeb>("mAOEAttackPoolEffectPrefeb"_hs)
		.prop(fq::reflect::prop::Name, "mAOEAttackPoolEffectPrefeb")
		.prop(fq::reflect::prop::Comment, u8"풀 이펙트 프리펩 경로")
		.base<fq::game_module::Component>();

	entt::meta<DebuffPoisonZone>()
		.type("DebuffPoisonZone"_hs)
		.prop(fq::reflect::prop::Name, "DebuffPoisonZone")
		.base<fq::game_module::Component>();

	entt::meta<ArrowAttack>()
		.type("ArrowAttack"_hs)
		.prop(fq::reflect::prop::Name, "ArrowAttack")
		.data<&ArrowAttack::mLifeTime>("LifeTime"_hs)
		.prop(fq::reflect::prop::Comment, u8"생존 시간")
		.prop(fq::reflect::prop::Name, "LifeTime")
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
		.data<&StaffSoulAttack::mKnockBackPower>("KnockBackPower"_hs)
		.prop(fq::reflect::prop::Name, "KnockBackPower")
		.data<&StaffSoulAttack::mAttack>("Attack"_hs)
		.prop(fq::reflect::prop::Name, "Attack")
		.base<fq::game_module::Component>();

	entt::meta<AttackInvalidation>()
		.type("AttackInvalidation"_hs)
		.prop(fq::reflect::prop::Name, "AttackInvalidation")
		.base<fq::game_module::Component>();


	//////////////////////////////////////////////////////////////////////////
	//                             UI										//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<HpBar>()
		.type("HpBar"_hs)
		.prop(fq::reflect::prop::Name, "HpBar")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&HpBar::mbIsVisible>("IsVisible"_hs)
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

	entt::meta<GaugeBar>()
		.type("GaugeBar"_hs)
		.prop(fq::reflect::prop::Name, "GaugeBar")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&GaugeBar::mbIsVisible>("IsVisible"_hs)
		.prop(fq::reflect::prop::Name, "IsVisible")
		.data<&GaugeBar::mBarSize>("BarSize"_hs)
		.prop(fq::reflect::prop::Name, "BarSize")
		.data<&GaugeBar::mWorldOffset>("WorldOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldOffset")
		.prop(fq::reflect::prop::Comment, u8"월드 공간의 Y를 더한후 UI 위치 계산")
		.data<&GaugeBar::mScreenOffset>("ScreenOffset"_hs)
		.prop(fq::reflect::prop::Name, "ScreenOffset")
		.prop(fq::reflect::prop::Comment, u8"Screen 공간의 Y를 더한후 UI 위치 계산")
		.data<&GaugeBar::mInnerOffset>("InnerOffset"_hs)
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
		.data<&PauseUI::mRepauseUIPrefab>("RepauseUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "RepauseUIPrefab")
		.data<&PauseUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<LoadingText>()
		.type("LoadingText"_hs)
		.prop(fq::reflect::prop::POD)
		.prop(fq::reflect::prop::Name, "LoadingText")
		.data<&LoadingText::Title>("Title"_hs)
		.prop(fq::reflect::prop::Name, "Title")
		.data<&LoadingText::Content>("Content"_hs)
		.prop(fq::reflect::prop::Name, "Content");

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
		.data<&SoulSelectUI::mSoulPrefab>("SoulPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SoulPrefab")
		.data<&SoulSelectUI::mMagicSymbolPrefab>("MagicSymbolPrefab"_hs)
		.prop(fq::reflect::prop::Name, "MagicSymbolPrefab")
		.data<&SoulSelectUI::mSoulMoveSpeed>("SoulMoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "SoulMoveSpeed")
		.prop(fq::reflect::prop::Comment, u8"레디 때 소환된 영혼 이동 속도")
		.data<&SoulSelectUI::mNextSceneName>("NextSceneName"_hs)
		.prop(fq::reflect::prop::Name, "NextSceneName")
		.base<fq::game_module::Component>();

	entt::meta<SettingUI>()
		.type("SettingUI"_hs)
		.prop(fq::reflect::prop::Name, "SettingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&SettingUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&SettingUI::mGameSettingPrefab>("GameSettingPrefab"_hs)
		.prop(fq::reflect::prop::Name, "GameSettingPrefab")
		.data<&SettingUI::mVideoSettingPrefab>("VideoSettingPrefab"_hs)
		.prop(fq::reflect::prop::Name, "VideoSettingPrefab")
		.data<&SettingUI::mAudioSettingPrefab>("AudioSettingPrefab"_hs)
		.prop(fq::reflect::prop::Name, "AudioSettingPrefab")
		.base<fq::game_module::Component>();

	entt::meta<RepauseUI>()
		.type("RepauseUI"_hs)
		.prop(fq::reflect::prop::Name, "RepauseUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<ResultUI>()
		.type("ResultUI"_hs)
		.prop(fq::reflect::prop::Name, "ResultUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<ResetPopupUI>()
		.type("ResetPopupUI"_hs)
		.prop(fq::reflect::prop::Name, "ResetPopupUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<SavePopupUI>()
		.type("SavePopupUI"_hs)
		.prop(fq::reflect::prop::Name, "SavePopupUI")
		.prop(fq::reflect::prop::Label, "UI")
		.base<fq::game_module::Component>();

	entt::meta<AudioSettingUI>()
		.type("AudioSettingUI"_hs)
		.prop(fq::reflect::prop::Name, "AudioSettingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&AudioSettingUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&AudioSettingUI::mResetMessagePrefab>("ResetMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "ResetMessagePrefab")
		.data<&AudioSettingUI::mSaveMessagePrefab>("SaveMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "SaveMessagePrefab")
		.data<&AudioSettingUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<GameSettingUI>()
		.type("GameSettingUI"_hs)
		.prop(fq::reflect::prop::Name, "GameSettingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&GameSettingUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&GameSettingUI::mResetMessagePrefab>("ResetMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "ResetMessagePrefab")
		.data<&GameSettingUI::mSaveMessagePrefab>("SaveMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "SaveMessagePrefab")
		.data<&GameSettingUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<VideoSettingUI>()
		.type("VideoSettingUI"_hs)
		.prop(fq::reflect::prop::Name, "VideoSettingUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&VideoSettingUI::mUIAnimSpeed>("UIAnimSpeed"_hs)
		.prop(fq::reflect::prop::Name, "UIAnimSpeed")
		.prop(fq::reflect::prop::Comment, u8"선택 버튼 이동 속도")
		.data<&VideoSettingUI::mResetMessagePrefab>("ResetMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "ResetMessagePrefab")
		.data<&VideoSettingUI::mSaveMessagePrefab>("SaveMessagePrefab"_hs)
		.prop(fq::reflect::prop::Name, "SaveMessagePrefab")
		.data<&VideoSettingUI::mSettingUIPrefab>("SettingUIPrefab"_hs)
		.prop(fq::reflect::prop::Name, "SettingUIPrefab")
		.base<fq::game_module::Component>();

	entt::meta<SpeechBubbleUI>()
		.type("SpeechBubbleUI"_hs)
		.prop(fq::reflect::prop::Name, "SpeechBubbleUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&SpeechBubbleUI::mName>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name")
		.data<&SpeechBubbleUI::mWorldOffset>("WorldOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldOffset")
		.data<&SpeechBubbleUI::mScreenOffset>("ScreenOffset"_hs)
		.prop(fq::reflect::prop::Name, "ScreenOffset")
		.data<&SpeechBubbleUI::mPlaytime>("Playtime"_hs)
		.prop(fq::reflect::prop::Name, "Playtime")
		.base<fq::game_module::Component>();

	entt::meta<BGaugeUI>()
		.type("BGaugeUI"_hs)
		.prop(fq::reflect::prop::Name, "BGaugeUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&BGaugeUI::mWorldOffset>("WorldOffset"_hs)
		.prop(fq::reflect::prop::Name, "WorldOffset")
		.data<&BGaugeUI::mGaugeSize>("GaugeSize"_hs)
		.prop(fq::reflect::prop::Name, "GaugeSize")
		.data<&BGaugeUI::mGaugeRatio>("GaugeRatio"_hs)
		.prop(fq::reflect::prop::Name, "GaugeRatio")
		.base<fq::game_module::Component>();

	entt::meta<PlayerCheckUI>()
		.type("PlayerCheckUI"_hs)
		.prop(fq::reflect::prop::Name, "PlayerCheckUI")
		.prop(fq::reflect::prop::Label, "UI")
		.data<&PlayerCheckUI::mOffset>("mOffset"_hs)
		.prop(fq::reflect::prop::Name, "mOffset")
		.data<&PlayerCheckUI::mWorldOffset>("mWorldOffset"_hs)
		.prop(fq::reflect::prop::Name, "mWorldOffset")
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

	entt::meta<ClearGoddessStatue>()
		.type("ClearGoddessStatue"_hs)
		.prop(fq::reflect::prop::Name, "ClearGoddessStatue")
		.prop(fq::reflect::prop::POD)
		.data<&ClearGoddessStatue::goddessStatueName>("GoddessStatueName"_hs)
		.prop(fq::reflect::prop::Name, "GoddessStatueName");

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
		.data<&QuestClearCondition::clearGoddessStatueList>("ClearGoddessStatueList"_hs)
		.prop(fq::reflect::prop::Name, "ClearGoddessStatueList");

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
	//                             Player Type								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<EPlayerType>()
		.prop(fq::reflect::prop::Name, "PlayerType")
		.conv<std::underlying_type_t<EPlayerType>>()
		.data<EPlayerType::None>("None"_hs)
		.prop(fq::reflect::prop::Name, "None")
		.data<EPlayerType::LivingArmour>("LivingArmour"_hs)
		.prop(fq::reflect::prop::Name, "LivingArmour")
		.data<EPlayerType::ArmourDestroyed>("ArmourDestroyed"_hs)
		.prop(fq::reflect::prop::Name, "ArmourDestroyed")
		.data<EPlayerType::Soul>("Soul"_hs)
		.prop(fq::reflect::prop::Name, "Soul")
		.data<EPlayerType::SoulDestoryed>("SoulDestoryed"_hs)
		.prop(fq::reflect::prop::Name, "SoulDestoryed");

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

		.data<&PlayerSoulVariable::SwordSoulColor>("SwordSoulColor"_hs)
		.prop(fq::reflect::prop::Name, "SwordSoulColor")
		.data<&PlayerSoulVariable::StaffSoulColor>("StaffSoulColor"_hs)
		.prop(fq::reflect::prop::Name, "StaffSoulColor")
		.data<&PlayerSoulVariable::AxeSoulColor>("AxeSoulColor"_hs)
		.prop(fq::reflect::prop::Name, "AxeSoulColor")
		.data<&PlayerSoulVariable::BowSoulColor>("BowSoulColor"_hs)
		.prop(fq::reflect::prop::Name, "BowSoulColor")

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

		.data<&DamageVariable::ArcherWACoefficient>("ArcherWACoefficient"_hs)
		.prop(fq::reflect::prop::Name, "ArcherWACoefficient")
		.data<&DamageVariable::ArcherCA_1_Coefficient>("ArcherCA_1_Coefficient"_hs)
		.prop(fq::reflect::prop::Name, "ArcherCA_1_Coefficient")
		.data<&DamageVariable::ArcherCA_2_Coefficient>("ArcherCA_2_Coefficient"_hs)
		.prop(fq::reflect::prop::Name, "ArcherCA_2_Coefficient")
		.data<&DamageVariable::ArcherCA_3_Coefficient>("ArcherCA_3_Coefficient"_hs)
		.prop(fq::reflect::prop::Name, "ArcherCA_3_Coefficient")
		.data<&DamageVariable::ArcherCA_4_Coefficient>("ArcherCA_4_Coefficient"_hs)
		.prop(fq::reflect::prop::Name, "ArcherCA_4_Coefficient")

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

		.data<&DamageVariable::BluntFirstConsecutiveAttack>("BluntFirstConsecutiveAttack"_hs)
		.prop(fq::reflect::prop::Name, "BluntFirstConsecutiveAttack")
		.prop(reflect::prop::Comment, u8"워리어 연속공격(1타) 배율")
		.data<&DamageVariable::BluntSecondConsecutiveAttack>("BluntSecondConsecutiveAttack"_hs)
		.prop(fq::reflect::prop::Name, "BluntSecondConsecutiveAttack")
		.prop(reflect::prop::Comment, u8"워리어 연속공격(2타) 배율")
		.data<&DamageVariable::BluntThirdConsecutiveAttack>("BluntThirdConsecutiveAttack"_hs)
		.prop(fq::reflect::prop::Name, "BluntThirdConsecutiveAttack")
		.prop(reflect::prop::Comment, u8"워리어 연속공격(3타) 배율")
		.data<&DamageVariable::BluntSwingAroundCoefficient>("BluntSwingAroundCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BluntSwingAroundCoefficient")
		.prop(reflect::prop::Comment, u8"워리어 휩쓸기 공격 배율")
		.data<&DamageVariable::BluntRushCoefficient>("BluntRushCoefficient"_hs)
		.prop(fq::reflect::prop::Name, "BluntRushCoefficient")
		.prop(reflect::prop::Comment, u8"워리어 돌진 공격 배율")
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

		.data<&SettingVariable::MuteMasterVolume>("MuteMasterVolume"_hs)
		.prop(fq::reflect::prop::Name, "MuteMasterVolume")
		.data<&SettingVariable::MuteBGMVolume>("MuteBGMVolume"_hs)
		.prop(fq::reflect::prop::Name, "MuteBGMVolume")
		.data<&SettingVariable::MuteSFXVolume>("MuteSFXVolume"_hs)
		.prop(fq::reflect::prop::Name, "MuteSFXVolume")
		.data<&SettingVariable::MuteVoiceVolume>("MuteVoiceVolume"_hs)
		.prop(fq::reflect::prop::Name, "MuteVoiceVolume")

		.data<&SettingVariable::IsVibe>("IsVibe"_hs)
		.prop(fq::reflect::prop::Name, "IsVibe")
		.data<&SettingVariable::IsUsedAimAssist>("IsUsedAimAssist"_hs)
		.prop(fq::reflect::prop::Name, "IsUsedAimAssist")
		.data<&SettingVariable::IsAllowOtherPlayerAttack>("IsAllowOtherPlayerAttack"_hs)
		.prop(fq::reflect::prop::Name, "IsAllowOtherPlayerAttack")

		.data<&SettingVariable::ArmourSpawnDistance>("ArmourSpawnDistance"_hs)
		.prop(fq::reflect::prop::Name, "ArmourSpawnDistance")
		.data<&SettingVariable::IsUseCameraInit>("IsUseCameraInit"_hs)
		.prop(fq::reflect::prop::Name, "IsUseCameraInit")
		.base<IGameVariable>();

	entt::meta<PlayerVariable>()
		.type("PlayerVariable"_hs)
		.prop(fq::reflect::prop::Name, "PlayerVariable")
		.data<&PlayerVariable::FeverAttackIncreaseRatio>("FeverAttackIncreaseRatio"_hs)
		.prop(fq::reflect::prop::Name, "FeverAttackIncreaseRatio")
		.data<&PlayerVariable::FeverSpeedIncreaseRatio>("FeverSpeedIncreaseRatio"_hs)
		.prop(fq::reflect::prop::Name, "FeverSpeedIncreaseRatio")
		.data<&PlayerVariable::HpReductionOnAttack>("HpReductionOnAttack"_hs)
		.prop(fq::reflect::prop::Name, "HpReductionOnAttack")
		.data<&PlayerVariable::HpReductionOnAttackMinHp>("HpReductionOnAttackMinHp"_hs)
		.prop(fq::reflect::prop::Name, "HpReductionOnAttackMinHp")
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

		.data<&PlayerInfoVariable::Player1State>("Player1State"_hs)
		.prop(fq::reflect::prop::Name, "Player1State")
		.data<&PlayerInfoVariable::Player2State>("Player2State"_hs)
		.prop(fq::reflect::prop::Name, "Player2State")
		.data<&PlayerInfoVariable::Player3State>("Player3State"_hs)
		.prop(fq::reflect::prop::Name, "Player3State")
		.data<&PlayerInfoVariable::Player4State>("Player4State"_hs)
		.prop(fq::reflect::prop::Name, "Player4State")

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

	entt::meta<SoulVariable>()
		.type("SoulVariable"_hs)
		.prop(fq::reflect::prop::Name, "SoulVariable")
		.data<&SoulVariable::ButtonTime>("ButtonTime"_hs)
		.prop(fq::reflect::prop::Name, "ButtonTime")
		.prop(reflect::prop::Comment, u8"영혼 이탈 시간")
		.data<&SoulVariable::OutTime>("OutTime"_hs)
		.prop(fq::reflect::prop::Name, "OutTime")
		.prop(reflect::prop::Comment, u8"갑옷 파괴 시, 게임 아웃 시간")
		.data<&SoulVariable::HpPercent>("HpPercent"_hs)
		.prop(fq::reflect::prop::Name, "HpPercent")
		.prop(reflect::prop::Comment, u8"갑옷 입고 있는 상태에서 이탈할 시에 갑옷 생성할 수 있는 최소 HP")

		.data<&SoulVariable::SoulMaxHp>("SoulMaxHp"_hs)
		.prop(fq::reflect::prop::Name, "SoulMaxHp")
		.prop(reflect::prop::Comment, u8"소울의 HP 최대치")
		.data<&SoulVariable::SoulMinHp>("SoulMinHp"_hs)
		.prop(fq::reflect::prop::Name, "SoulMinHp")
		.prop(reflect::prop::Comment, u8"사망 시 영혼 최대 체력이 줄어드는데, 일정 사망 횟수 이상에서도 최소 HP값에 도달하면 최대 HP값이 줄어들지 않는 변수")
		.data<&SoulVariable::SoulHpDown>("SoulHpDown"_hs)
		.prop(fq::reflect::prop::Name, "SoulHpDown")
		.prop(reflect::prop::Comment, u8"영혼 파괴 시, 최대 체력 감소량")
		.data<&SoulVariable::SoulHpDecreas>("SoulHpDecreas"_hs)
		.prop(fq::reflect::prop::Name, "SoulHpDecreas")
		.prop(reflect::prop::Comment, u8"초당 HP가 감소하는 양")
		.data<&SoulVariable::SoulDistance>("SoulDistance"_hs)
		.prop(fq::reflect::prop::Name, "SoulDistance")
		.prop(reflect::prop::Comment, u8"영혼 HP 피해 감소 및 영혼의 갑옷 버프 사정거리")
		.data<&SoulVariable::SoulDecreasPercentage>("SoulDecreasPercentage"_hs)
		.prop(fq::reflect::prop::Name, "SoulDecreasPercentage")
		.prop(reflect::prop::Comment, u8"사정거리 안에 갑옷 플레이어가 있을 때, HP 피해 감소량")

		.data<&SoulVariable::SpeedUpRatio>("SpeedUp"_hs)
		.prop(fq::reflect::prop::Name, "SpeedUp")
		.prop(reflect::prop::Comment, u8"플레이어 이동속도 증가량")
		.data<&SoulVariable::DamageUpRatio>("DamageUp"_hs)
		.prop(fq::reflect::prop::Name, "DamageUp")
		.prop(reflect::prop::Comment, u8"플레이어 공격력 증가량")

		.data<&SoulVariable::Player1Type>("Player1Type"_hs)
		.prop(fq::reflect::prop::Name, "Player1Type")
		.data<&SoulVariable::Player1DeathCount>("Player1DeathCount"_hs)
		.prop(fq::reflect::prop::Name, "Player1DeathCount")
		.data<&SoulVariable::Player2Type>("Player2Type"_hs)
		.prop(fq::reflect::prop::Name, "Player2Type")
		.data<&SoulVariable::Player2DeathCount>("Player2DeathCount"_hs)
		.prop(fq::reflect::prop::Name, "Player2DeathCount")
		.data<&SoulVariable::Player3Type>("Player3Type"_hs)
		.prop(fq::reflect::prop::Name, "Player3Type")
		.data<&SoulVariable::Player3DeathCount>("Player3DeathCount"_hs)
		.prop(fq::reflect::prop::Name, "Player3DeathCount")
		.data<&SoulVariable::Player4Type>("Player4Type"_hs)
		.prop(fq::reflect::prop::Name, "Player4Type")
		.data<&SoulVariable::Player4DeathCount>("Player4DeathCount"_hs)
		.prop(fq::reflect::prop::Name, "Player4DeathCount")
		.base<IGameVariable>();

	entt::meta<LevelVariable>()
		.type("LevelVariable"_hs)
		.prop(fq::reflect::prop::Name, "LevelVariable")
		.data<&LevelVariable::EasyDamage>("EasyDamage"_hs)
		.prop(fq::reflect::prop::Name, "EasyDamage")
		.data<&LevelVariable::EasyHp>("EasyHp"_hs)
		.prop(fq::reflect::prop::Name, "EasyHp")

		.data<&LevelVariable::NormalDamage>("NormalDamage"_hs)
		.prop(fq::reflect::prop::Name, "NormalDamage")
		.data<&LevelVariable::NormalHp>("NormalHp"_hs)
		.prop(fq::reflect::prop::Name, "NormalHp")

		.data<&LevelVariable::HardDamage>("HardDamage"_hs)
		.prop(fq::reflect::prop::Name, "HardDamage")
		.data<&LevelVariable::HardHp>("HardHp"_hs)
		.prop(fq::reflect::prop::Name, "HardHp")

		.data<&LevelVariable::Player2Damage>("Player2Damage"_hs)
		.prop(fq::reflect::prop::Name, "Player2Damage")
		.data<&LevelVariable::Player2Hp>("Player2Hp"_hs)
		.prop(fq::reflect::prop::Name, "Player2Hp")
		.data<&LevelVariable::Player2Spawn>("Player2Spawn"_hs)
		.prop(fq::reflect::prop::Name, "Player2Spawn")

		.data<&LevelVariable::Player3Damage>("Player3Damage"_hs)
		.prop(fq::reflect::prop::Name, "Player3Damage")
		.data<&LevelVariable::Player3Hp>("Player3Hp"_hs)
		.prop(fq::reflect::prop::Name, "Player3Hp")
		.data<&LevelVariable::Player3Spawn>("Player3Spawn"_hs)
		.prop(fq::reflect::prop::Name, "Player3Spawn")

		.data<&LevelVariable::Player4Damage>("Player4Damage"_hs)
		.prop(fq::reflect::prop::Name, "Player4Damage")
		.data<&LevelVariable::Player4Spawn>("Player4Spawn"_hs)
		.prop(fq::reflect::prop::Name, "Player4Spawn")
		.data<&LevelVariable::Player4Hp>("Player4Hp"_hs)
		.prop(fq::reflect::prop::Name, "Player4Hp")

		.base<IGameVariable>();

	entt::meta<MonsterVariable>()
		.type("MonsterVariable"_hs)
		.prop(fq::reflect::prop::Name, "MonsterVariable")
		.data<&MonsterVariable::OnRagdoll>("OnRagdoll"_hs)
		.prop(fq::reflect::prop::Name, "OnRagdoll")
		.data<&MonsterVariable::MaxRagdollsPerScene>("MaxRagdollsPerScene"_hs)
		.prop(fq::reflect::prop::Name, "MaxRagdollsPerScene")
		.data<&MonsterVariable::MinFrameCountForRagdoll>("MinFrameCountForRagdoll"_hs)
		.prop(fq::reflect::prop::Name, "MinFrameCountForRagdoll")
		.data<&MonsterVariable::MaxOneFrameCreateRagdollCount>("MaxOneFrameCreateRagdollCount"_hs)
		.prop(fq::reflect::prop::Name, "MaxOneFrameCreateRagdollCount")
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
	//								  Suport								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<YAxisSetter>()
		.type("YAxisSetter"_hs)
		.prop(reflect::prop::Name, "YAxisSetter")
		.data<&YAxisSetter::SetOffsetYAxis, &YAxisSetter::GetOffsetYAxis>("OffsetYAxis"_hs)
		.prop(reflect::prop::Name, "OffsetYAxis")
		.prop(reflect::prop::Comment, u8"RayCast 시작할 YOffset값 입니다.")
		.data<&YAxisSetter::SetDistance, &YAxisSetter::GetDistance>("Distance"_hs)
		.prop(reflect::prop::Name, "Distance")
		.prop(reflect::prop::Comment, u8"RayCast 거리 값 입니다.")
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

	entt::meta<InProgressQuest>()
		.type("InProgressQuest"_hs)
		.prop(fq::reflect::prop::Name, "InProgressQuest")
		.prop(fq::reflect::prop::POD)
		.data<&InProgressQuest::isMain>("isMain"_hs)
		.prop(fq::reflect::prop::Name, "isMain")
		.data<&InProgressQuest::QuestIndex>("QuestIndex"_hs)
		.prop(fq::reflect::prop::Name, "QuestIndex");

	entt::meta<InProgressDefence>()
		.type("InProgressDefence"_hs)
		.prop(fq::reflect::prop::Name, "InProgressDefence")
		.prop(fq::reflect::prop::POD)
		.data<&InProgressDefence::ColliderName>("ColliderName"_hs)
		.prop(fq::reflect::prop::Name, "ColliderName")
		.data<&InProgressDefence::Count>("Count"_hs)
		.prop(fq::reflect::prop::Name, "Count");

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
		.prop(fq::reflect::prop::Name, "ObjectLiveList")
		.data<&SpawnCondition::InProgressQuestList>("InProgressQuestList"_hs)
		.prop(fq::reflect::prop::Name, "InProgressQuestList")
		.data<&SpawnCondition::InProgressDefenceList>("InProgressDefenceList"_hs)
		.prop(fq::reflect::prop::Name, "InProgressDefenceList");

	entt::meta<SpawnData>()
		.type("SpawnData"_hs)
		.prop(fq::reflect::prop::Name, "SpawnData")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnData::MonsterType>("MonsterType"_hs)
		.prop(fq::reflect::prop::Name, "MonsterType")
		.data<&SpawnData::Name>("Name"_hs)
		.prop(fq::reflect::prop::Name, "Name")
		.data<&SpawnData::SpawnerNum>("SpawnerNum"_hs)
		.prop(fq::reflect::prop::Name, "SpawnerNum")
		.data<&SpawnData::SpawnMonsterNum>("SpawnMonsterNum"_hs)
		.prop(fq::reflect::prop::Name, "SpawnMonsterNum");

	entt::meta<SpawnRule>()
		.type("SpawnRule"_hs)
		.prop(fq::reflect::prop::Name, "SpawnRule")
		.prop(fq::reflect::prop::POD)
		.data<&SpawnRule::spawnData>("SpawnData"_hs)
		.prop(fq::reflect::prop::Name, "SpawnData");

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
