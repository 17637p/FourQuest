#include "Resgistration.h"
#include <directxtk/SimpleMath.h>

#include "../FQGameModule/GameModule.h"
#include "../FQReflect/FQReflect.h"

fq::game_engine::Resgistration::Resgistration()
{

}

FQ_REGISTRATION
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

	// Vector2
	entt::meta<DirectX::SimpleMath::Vector2>()
		.type(entt::hashed_string("Vector2"))
		.data<&DirectX::SimpleMath::Vector2::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::name, "x")
		.data<&DirectX::SimpleMath::Vector2::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::name, "y");

	// Vector3
	entt::meta<DirectX::SimpleMath::Vector3>()
		.type(entt::hashed_string("Vector3"))
		.data<&DirectX::SimpleMath::Vector3::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::name, "x")
		.data<&DirectX::SimpleMath::Vector3::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::name, "y")
		.data<&DirectX::SimpleMath::Vector3::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::name, "z");

	// Vector4
	entt::meta<DirectX::SimpleMath::Vector4>()
		.type(entt::hashed_string("Vector4"))
		.data<&DirectX::SimpleMath::Vector4::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::name, "x")
		.data<&DirectX::SimpleMath::Vector4::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::name, "y")
		.data<&DirectX::SimpleMath::Vector4::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::name, "z")
		.data<&DirectX::SimpleMath::Vector4::w>(entt::hashed_string("w"))
		.prop(fq::reflect::prop::name, "w");

	// Quaternion
	entt::meta<DirectX::SimpleMath::Quaternion>()
		.type(entt::hashed_string("Quaternion"))
		.data<&DirectX::SimpleMath::Quaternion::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::name, "x")
		.data<&DirectX::SimpleMath::Quaternion::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::name, "y")
		.data<&DirectX::SimpleMath::Quaternion::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::name, "z")
		.data<&DirectX::SimpleMath::Quaternion::w>(entt::hashed_string("w"))
		.prop(fq::reflect::prop::name, "w");
}

