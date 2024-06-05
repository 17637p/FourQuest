
#include "ClientRegistration.h"

#include "../FQReflect/FQReflect.h"
#include "PlayerMovement.h"
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

	entt::meta<PlayerMovement>()
		.type("PlayerMovement"_hs)
		.prop(reflect::prop::Name, "PlayerMovement")
		.base<game_module::Component>();

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

