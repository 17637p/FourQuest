#pragma once

#include "entt.hpp"

namespace fq::reflect::tag
{
	constexpr static entt::hashed_string name = entt::hashed_string("name"); // 변수,함수 이름
	constexpr static entt::hashed_string label = entt::hashed_string("label"); 
	constexpr static entt::hashed_string comment = entt::hashed_string("comment"); // 주석
}
