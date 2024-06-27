#include "FQGameEngineRegister.h"
#include <directxtk/SimpleMath.h>
#include <filesystem>

#include "../FQGameModule/GameModule.h"
#include "../FQReflect/FQReflect.h"

void fq::game_engine::RegisterMetaData()
{
	using namespace entt::literals;

	entt::meta<DirectX::SimpleMath::Matrix>()
		.type(entt::hashed_string("Matrix"))
		.data<&DirectX::SimpleMath::Matrix::_11>(entt::hashed_string("_11"))
		.prop(fq::reflect::prop::Name, "_11")
		.data<&DirectX::SimpleMath::Matrix::_12>(entt::hashed_string("_12"))
		.prop(fq::reflect::prop::Name, "_12")
		.data<&DirectX::SimpleMath::Matrix::_13>(entt::hashed_string("_13"))
		.prop(fq::reflect::prop::Name, "_13")
		.data<&DirectX::SimpleMath::Matrix::_14>(entt::hashed_string("_14"))
		.prop(fq::reflect::prop::Name, "_14")
		.data<&DirectX::SimpleMath::Matrix::_21>(entt::hashed_string("_21"))
		.prop(fq::reflect::prop::Name, "_21")
		.data<&DirectX::SimpleMath::Matrix::_22>(entt::hashed_string("_22"))
		.prop(fq::reflect::prop::Name, "_22")
		.data<&DirectX::SimpleMath::Matrix::_23>(entt::hashed_string("_23"))
		.prop(fq::reflect::prop::Name, "_23")
		.data<&DirectX::SimpleMath::Matrix::_24>(entt::hashed_string("_24"))
		.prop(fq::reflect::prop::Name, "_24")
		.data<&DirectX::SimpleMath::Matrix::_31>(entt::hashed_string("_31"))
		.prop(fq::reflect::prop::Name, "_31")
		.data<&DirectX::SimpleMath::Matrix::_32>(entt::hashed_string("_32"))
		.prop(fq::reflect::prop::Name, "_32")
		.data<&DirectX::SimpleMath::Matrix::_33>(entt::hashed_string("_33"))
		.prop(fq::reflect::prop::Name, "_33")
		.data<&DirectX::SimpleMath::Matrix::_34>(entt::hashed_string("_34"))
		.prop(fq::reflect::prop::Name, "_34")
		.data<&DirectX::SimpleMath::Matrix::_41>(entt::hashed_string("_41"))
		.prop(fq::reflect::prop::Name, "_41")
		.data<&DirectX::SimpleMath::Matrix::_42>(entt::hashed_string("_42"))
		.prop(fq::reflect::prop::Name, "_42")
		.data<&DirectX::SimpleMath::Matrix::_43>(entt::hashed_string("_43"))
		.prop(fq::reflect::prop::Name, "_43")
		.data<&DirectX::SimpleMath::Matrix::_44>(entt::hashed_string("_44"))
		.prop(fq::reflect::prop::Name, "_44");


	// Vector2
	entt::meta<DirectX::SimpleMath::Vector2>()
		.type(entt::hashed_string("Vector2"))
		.prop(fq::reflect::prop::Name, "Vector2")
		.prop(fq::reflect::prop::POD)
		.data<&DirectX::SimpleMath::Vector2::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::Name, "x")
		.data<&DirectX::SimpleMath::Vector2::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::Name, "y");

	// Vector3
	entt::meta<DirectX::SimpleMath::Vector3>()
		.type(entt::hashed_string("Vector3"))
		.prop(fq::reflect::prop::Name, "Vector3")
		.prop(fq::reflect::prop::POD)
		.data<&DirectX::SimpleMath::Vector3::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::Name, "x")
		.data<&DirectX::SimpleMath::Vector3::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::Name, "y")
		.data<&DirectX::SimpleMath::Vector3::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::Name, "z");

	// Vector4
	entt::meta<DirectX::SimpleMath::Vector4>()
		.type(entt::hashed_string("Vector4"))
		.prop(fq::reflect::prop::Name, "Vector4")
		.prop(fq::reflect::prop::POD)
		.data<&DirectX::SimpleMath::Vector4::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::Name, "x")
		.data<&DirectX::SimpleMath::Vector4::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::Name, "y")
		.data<&DirectX::SimpleMath::Vector4::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::Name, "z")
		.data<&DirectX::SimpleMath::Vector4::w>(entt::hashed_string("w"))
		.prop(fq::reflect::prop::Name, "w");

	// Quaternion
	entt::meta<DirectX::SimpleMath::Quaternion>()
		.type(entt::hashed_string("Quaternion"))
		.data<&DirectX::SimpleMath::Quaternion::x>(entt::hashed_string("x"))
		.prop(fq::reflect::prop::Name, "x")
		.data<&DirectX::SimpleMath::Quaternion::y>(entt::hashed_string("y"))
		.prop(fq::reflect::prop::Name, "y")
		.data<&DirectX::SimpleMath::Quaternion::z>(entt::hashed_string("z"))
		.prop(fq::reflect::prop::Name, "z")
		.data<&DirectX::SimpleMath::Quaternion::w>(entt::hashed_string("w"))
		.prop(fq::reflect::prop::Name, "w");

	// Color
	entt::meta<DirectX::SimpleMath::Color>()
		.type("Color"_hs)
		.data<&DirectX::SimpleMath::Color::x>("x"_hs)
		.prop(fq::reflect::prop::Name, "x")
		.data<&DirectX::SimpleMath::Color::y>("y"_hs)
		.prop(fq::reflect::prop::Name, "y")
		.data<&DirectX::SimpleMath::Color::z>("z"_hs)
		.prop(fq::reflect::prop::Name, "z")
		.data<&DirectX::SimpleMath::Color::w>("w"_hs)
		.prop(fq::reflect::prop::Name, "w");
}
