
#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"
#include "Player.h"
#include "PlayerInputState.h"
#include "PlayerMovementState.h"
#include "PlayerDashState.h"

#include "Monster.h"
#include "MonsterIdle.h"
#include "MonsterFind.h"
#include "MonsterChase.h"
#include "MonsterDamaged.h"
#include "MonsterAttack.h"
#include "MonsterDie.h"

void fq::client::RegisterMetaData()
{
	using namespace entt::literals;

	//////////////////////////////////////////////////////////////////////////
	//                             플레이어									//
	//////////////////////////////////////////////////////////////////////////

	entt::meta<Player>()
		.type("Player"_hs)
		.prop(reflect::prop::Name, "Player")
		.data<&Player::SetHp, &Player::GetHp>("HP"_hs)
		.prop(reflect::prop::Name, "Hp")
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
		.data<&Monster::SetMoveSpeed, &Monster::GetMoveSpeed>("MoveSpeed"_hs)
		.prop(fq::reflect::prop::Name, "MoveSpeed")
		.data<&Monster::SetChaseDistance, &Monster::GetChaseDistance>("ChaseDistance"_hs)
		.prop(fq::reflect::prop::Name, "ChaseDistance")
		.data<&Monster::SetAttackWaitTime, &Monster::GetAttackWaitTime>("AttackWaitTime"_hs)
		.prop(fq::reflect::prop::Name, "AttackWaitTime")
		.data<&Monster::SetTargetAttackRange, &Monster::GetTargetAttackRange>("TargetAttackRange"_hs)
		.prop(fq::reflect::prop::Name, "TargetAttackRange")
		.base<fq::game_module::Component>();

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


}

