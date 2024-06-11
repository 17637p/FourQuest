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

	// 리소스 드래그 드랍 설정 
	// value를 ".mp3/.wav"로 설정하면 mp3, wav 파일을 드래그 드랍 받기 가능합니다 
	constexpr static entt::hashed_string DragDrop = entt::hashed_string("drag_drop");

	// Plain Old Data Structure 순수한 변수들의 집합
	constexpr static entt::hashed_string POD = entt::hashed_string("POD");

	// 컴포넌트 라벨 설정 
	constexpr static entt::hashed_string Label = entt::hashed_string("label"); 
}
