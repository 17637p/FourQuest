#pragma once

#include "entt.hpp"

namespace fq::reflect::prop
{
	// 함수, 변수 이름
	constexpr static entt::hashed_string Name = entt::hashed_string("name");

	// 주석
	constexpr static entt::hashed_string Comment = entt::hashed_string("comment"); 	

	// 상대경로로 저장 설정
	constexpr static entt::hashed_string RelativePath = entt::hashed_string("relative_path");

	// 읽기 전용
	constexpr static entt::hashed_string ReadOnly = entt::hashed_string("read_only");

	// ??
	constexpr static entt::hashed_string Label = entt::hashed_string("label"); 
}
