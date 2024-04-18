#include "GameModuleRegister.h"

#include "../FQReflect/FQReflect.h"
#include "GameModuleEnum.h"

void fq::game_module::RegisterMetaData()
{
	using namespace entt::literals;

	entt::meta<fq::game_module::Tag>()
		.prop(fq::reflect::prop::name, "Tag")
		.conv<std::underlying_type_t<fq::game_module::Tag>>()
		.data<fq::game_module::Tag::Untagged>("Untagged"_hs)
		.prop(fq::reflect::prop::name, "Untagged")
		.data<fq::game_module::Tag::Camera>("Camera"_hs)
		.prop(fq::reflect::prop::name, "Camera")
		.data<fq::game_module::Tag::Player>("Player"_hs)
		.prop(fq::reflect::prop::name, "Player");
}
