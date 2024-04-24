#pragma once

#include "entt.hpp"

namespace fq::reflect::prop
{
	// 함수, 변수 이름
	constexpr static entt::hashed_string name = entt::hashed_string("name");
	// 주석
	constexpr static entt::hashed_string comment = entt::hashed_string("comment"); 	
	// 상대경로로 저장 설정
	constexpr static entt::hashed_string relative_path = entt::hashed_string("relative_path");

	// 읽기 전용
	constexpr static entt::hashed_string read_only = entt::hashed_string("read_only");

	// ??
	constexpr static entt::hashed_string label = entt::hashed_string("label"); 
}
