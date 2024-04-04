#include "GameModulePCH.h"
#include "GameModuleEnum.h"

FQ_REGISTRATION
{
	entt::meta<fq::game_module::Tag>()
		.data<fq::game_module::Tag::Untagged>(entt::hashed_string("Untagged"))
		.prop(fq::reflect::tag::name, "Untagged")
		.data<fq::game_module::Tag::Camera>(entt::hashed_string("Camera"))
		.prop(fq::reflect::tag::name, "Camera")
		.data<fq::game_module::Tag::Player>(entt::hashed_string("Player"))
		.prop(fq::reflect::tag::name, "Player");
}