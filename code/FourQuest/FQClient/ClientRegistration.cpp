
#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"
#include "Player.h"
#include "Soul.h"
#include "PlayerInputState.h"
#include "PlayerMovementState.h"
#include "PlayerDashState.h"
#include "PlayerAttackState.h"
#include "DeadArmour.h"

// Monster
#include "Monster.h"
#include "MonsterIdle.h"
#include "MonsterFind.h"
#include "MonsterChase.h"
#include "MonsterDamaged.h"
#include "MonsterAttack.h"
#include "MonsterDie.h"

// MonsterSpawner
#include "MonsterSpawner.h"

#include "Attack.h"

// UI
#include "HpBar.h"

#include "CameraMoving.h"

void fq::client::RegisterMetaData()
{
	using namespace entt::literals;

	//////////////////////////////////////////////////////////////////////////
	//                             플레이어 관련								//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Player>()
		.type("Player"_hs)
		.prop(reflect::prop::Name, "Player")
		.prop(reflect::prop::Label, "Player")
		.data<&Player::mHp>("Hp"_hs)
		.prop(reflect::prop::Name, "Hp")
		.data<&Player::mDashCoolTime>("DashCoolTime"_hs)
		.prop(reflect::prop::Name, "DashCoolTime")
		.data<&Player::mInvincibleTime>("InvincibleTime"_hs)
		.prop(reflect::prop::Name, "InvincibleTime")
		.prop(reflect::prop::Comment, u8"무적시간")
		.data<&Player::mAttackPrafab>("AttakPrefab"_hs)
		.prop(reflect::prop::Name, "AttakPrefab")
		.data<&Player::mSoulPrefab>("SoulPrefab"_hs)
		.prop(reflect::prop::Name, "SoulPrefab")
		.data<&Player::mAttackRange>("AttackRange"_hs)
		.prop(reflect::prop::Name, "AttackRange")
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

	entt::meta<Soul>()
		.type("Soul"_hs)
		.prop(reflect::prop::Name, "Soul")
		.prop(reflect::prop::Label, "Player")
		.base<game_module::Component>();

	entt::meta<PlayerInputState>()
		.type("PlayerInputState"_hs)
		.prop(reflect::prop::Name, "PlayerInputState")
		.base<game_module::IStateBehaviour>();

	entt::meta<PlayerMovementState>()
		.type("PlayerMovementState"_hs)
		.prop(reflect::prop::Name, "PlayerMovementState")
		.data<&PlayerMovementState::SetMovePlayer, &PlayerMovementState::CanMovePlayer>("CanMovePlayer"_hs)
		.prop(reflect::prop::Name, "CanMovePlayer")
		.base<game_module::IStateBehaviour>();

	entt::meta<PlayerDashState>()
		.type("PlayerDashState"_hs)
		.prop(reflect::prop::Name, "PlayerDashState")
		.data<&PlayerDashState::SetDashPower, &PlayerDashState::GetDashPower>("DashPower"_hs)
		.prop(reflect::prop::Name, "DashPower")
		.base<game_module::IStateBehaviour>();

	entt::meta<PlayerAttackState>()
		.type("PlayerAttackState"_hs)
		.prop(reflect::prop::Name, "PlayerAttackState")
		.data<&PlayerAttackState::mAttackRebound>("AttackRebound"_hs)
		.prop(reflect::prop::Name, "AttackRebound")
		.prop(reflect::prop::Comment, u8"공격 반동")
		.base<game_module::IStateBehaviour>();

	//////////////////////////////////////////////////////////////////////////
	//                             몬스터									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Monster>()
		.type("Monster"_hs)
		.prop(fq::reflect::prop::Name, "Monster")
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

	entt::meta<Attack>()
		.type("Attack"_hs)
		.prop(fq::reflect::prop::Name, "Attack")
		.data<&Attack::mAttackTime>("AttackTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackTime")
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
	//                             몬스터 스포너	 							//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<MonsterSpawner>()
		.type("MonsterSpawner"_hs)
		.prop(fq::reflect::prop::Name, "MonsterSpawner")
		.data<&MonsterSpawner::mMonster>("MonsterPrefab"_hs)
		.prop(fq::reflect::prop::Name, "MonsterPrefab")
		.data<&MonsterSpawner::mSpawnCoolTime>("SpwanCoolTime"_hs)
		.prop(fq::reflect::prop::Name, "SpwanCoolTime")
		.base<fq::game_module::Component>();

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
}


