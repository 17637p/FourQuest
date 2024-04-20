#include "GameModuleRegister.h"

#include "../FQReflect/FQReflect.h"
#include "GameModuleEnum.h"
#include "GameObject.h"

void fq::game_module::RegisterMetaData()
{
	using namespace entt::literals;

	// Tag
	entt::meta<fq::game_module::Tag>()
		.prop(fq::reflect::prop::name, "Tag")
		.conv<std::underlying_type_t<fq::game_module::Tag>>()
		.data<fq::game_module::Tag::Untagged>("Untagged"_hs)
		.prop(fq::reflect::prop::name, "Untagged")
		.data<fq::game_module::Tag::Camera>("Camera"_hs)
		.prop(fq::reflect::prop::name, "Camera")
		.data<fq::game_module::Tag::Player>("Player"_hs)
		.prop(fq::reflect::prop::name, "Player");

	// GameObject 
	entt::meta<fq::game_module::GameObject>()
		.type(entt::hashed_string("GameObject"))
		.prop(fq::reflect::prop::name, "GameObject")
		.data<&fq::game_module::GameObject::SetName, &fq::game_module::GameObject::GetName>(entt::hashed_string("mName"))
		.prop(fq::reflect::prop::name, "mName")
		.data<&fq::game_module::GameObject::SetTag, &fq::game_module::GameObject::GetTag>(entt::hashed_string("mTag"))
		.prop(fq::reflect::prop::name, "mTag");
}
