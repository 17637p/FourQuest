#pragma once

#include "entt.hpp"

namespace fq::reflect::prop
{
	// �Լ�, ���� �̸�
	constexpr static entt::hashed_string Name = entt::hashed_string("name");

	// �ּ�
	constexpr static entt::hashed_string Comment = entt::hashed_string("comment"); 	

	// ����η� ���� ����
	constexpr static entt::hashed_string RelativePath = entt::hashed_string("relative_path");

	// �б� ����
	constexpr static entt::hashed_string ReadOnly = entt::hashed_string("read_only");

	// ??
	constexpr static entt::hashed_string Label = entt::hashed_string("label"); 
}
