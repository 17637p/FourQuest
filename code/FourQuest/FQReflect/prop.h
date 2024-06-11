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

	// ���ҽ� �巡�� ��� ���� 
	// value�� ".mp3/.wav"�� �����ϸ� mp3, wav ������ �巡�� ��� �ޱ� �����մϴ� 
	constexpr static entt::hashed_string DragDrop = entt::hashed_string("drag_drop");

	// Plain Old Data Structure ������ �������� ����
	constexpr static entt::hashed_string POD = entt::hashed_string("POD");

	// ������Ʈ �� ���� 
	constexpr static entt::hashed_string Label = entt::hashed_string("label"); 
}
