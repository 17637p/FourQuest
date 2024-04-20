#include "GameModuleRegister.h"

#include "../FQReflect/FQReflect.h"
#include "GameModuleEnum.h"
#include "GameObject.h"
#include "Transform.h"
#include "Camera.h"

void fq::game_module::RegisterMetaData()
{
	using namespace entt::literals;
	using namespace fq::game_module;

	// Tag
	entt::meta<Tag>()
		.prop(fq::reflect::prop::name, "Tag")
		.conv<std::underlying_type_t<Tag>>()
		.data<Tag::Untagged>("Untagged"_hs)
		.prop(fq::reflect::prop::name, "Untagged")
		.data<Tag::Camera>("Camera"_hs)
		.prop(fq::reflect::prop::name, "Camera")
		.data<Tag::Player>("Player"_hs)
		.prop(fq::reflect::prop::name, "Player");

	// GameObject 
	entt::meta<GameObject>()
		.type("GameObject"_hs)
		.prop(fq::reflect::prop::name, "GameObject")
		.data<&GameObject::SetName, &GameObject::GetName>("mName"_hs)
		.prop(fq::reflect::prop::name, "mName")
		.data<&GameObject::SetTag, &GameObject::GetTag>("mTag"_hs)
		.prop(fq::reflect::prop::name, "mTag");

	// Transform
	entt::meta<Transform>()
		.type("Transform"_hs)
		.prop(fq::reflect::prop::name, "Transform")
		.data<&Transform::SetLocalPosition, &Transform::GetLocalPosition>("Position"_hs)
		.prop(fq::reflect::prop::name, "Position")
		.prop(fq::reflect::prop::comment, u8"·ÎÄÃ ÁÂÇ¥")
		.data<&Transform::SetLocalRotation, &Transform::GetLocalRotation>("Rotation"_hs)
		.prop(fq::reflect::prop::name, "Rotation")
		.data<&Transform::SetLocalScale, &Transform::GetLocalScale>("Scale"_hs)
		.prop(fq::reflect::prop::name, "Scale")
		.base<Component>();

	// Camera
	entt::meta<Camera>()
		.type("Camera"_hs)
		.prop(fq::reflect::prop::name, "Camera")
		.data<&Camera::SetFieldOfView, &Camera::GetFieldOfView>("FOV"_hs)
		.prop(fq::reflect::prop::name, "FOV")
		.prop(fq::reflect::prop::comment, "0 ~ 180")
		.data<&Camera::SetFarPlain, &Camera::GetFarPlain>("FarPlain"_hs)
		.prop(fq::reflect::prop::name, "FarPlain")
		.data<&Camera::SetNearPlain, &Camera::GetNearPlain>("NearPlain"_hs)
		.prop(fq::reflect::prop::name, "NearPlain")
		.base<Component>();
}
