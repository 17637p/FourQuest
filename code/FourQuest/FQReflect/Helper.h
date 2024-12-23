#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "entt.hpp"

namespace fq::reflect
{
	/// <summary>
	/// meta_data 의 prop중 name을 반환합니다
	/// </summary>
	/// <param name="metaData">메타데이터</param>
	/// <returns>name 정보가 없으면 빈 string을 반환합니다</returns>
	std::string GetName(const entt::meta_data& metaData);

	/// <summary>
	/// meta_type 의 prop중 name을 반환합니다
	/// </summary>
	/// <param name="metaType">메타타입</param>
	/// <returns>name 정보가 없으면  빈 string을 반환합니다</returns>
	std::string GetName(const entt::meta_type& metaType);

	/// <summary>
	/// meta_data 의 prop중 comment를 반환합니다
	/// </summary>
	/// <param name="metaData">메타데이터</param>
	/// <returns>comment 정보가 없으면 빈 string을 반환합니다</returns>
	std::string GetComment(const entt::meta_data& metaData);
	
	/// <summary>
	/// meta_type의 prop중 label을 반환합니다.
	/// </summary>
	/// <param name="metaType">메타타입</param>
	/// <returns>label이 정보가 없으면 빈 string을 반환합니다</returns>
	std::string GetLabel(const entt::meta_type& metaType);

	/// <summary>
	/// 읽기전용 타입인지
	/// </summary>
	bool IsReadOnly(const entt::meta_data& data);

	/// <summary>
	/// fq::reflect::prop::DragDrop을 설정한 확장자명들을 반환합니다
	/// </summary>
	std::vector<std::filesystem::path> GetDragDropExtension(const entt::meta_data& data);
}

