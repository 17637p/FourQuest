#pragma once

#include "entt.hpp"

namespace fq::reflect::prop
{
	// �Լ�, ���� �̸�
	constexpr static entt::hashed_string name = entt::hashed_string("name");
	// �ּ�
	constexpr static entt::hashed_string comment = entt::hashed_string("comment"); 	
	// ����η� ���� ����
	constexpr static entt::hashed_string relative_path = entt::hashed_string("relative_path");

	// �б� ����
	constexpr static entt::hashed_string read_only = entt::hashed_string("read_only");

	// ??
	constexpr static entt::hashed_string label = entt::hashed_string("label"); 
}
