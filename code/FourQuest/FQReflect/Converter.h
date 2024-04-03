#pragma once

#include <filesystem>

#include "entt.hpp"
#include "json.hpp"
#include "json_fwd.hpp"

namespace fq::reflect
{
	/// <summary>
	/// 직렬화 클래스
	/// </summary>
	class Converter
	{
	public:
		/// <summary>
		/// class 객체를 json 형식의 파일로 직렬화
		/// </summary>
		/// <param name="path">파일 경로</param>
		/// <param name="data">any 객체로 감싼 class</param>
		void SerializeClass(const std::filesystem::path& path, const entt::meta_any& object);

		/// <summary>
		/// json 형식의 파일을 any 객체로 역직렬화
		/// </summary>
		/// <param name="path">읽을 파일 경로</param>
		/// <returns>역직렬화한 any 객체</returns>
		entt::meta_any DeserializeClass(const std::filesystem::path& path);

	private:
		// Instance -> Json 
		void parseClassToJson(const entt::meta_any& object, nlohmann::json& outJson);
		void parseMemberToJson(const entt::meta_data& metaData, const entt::meta_any& object, nlohmann::json& outJson);

		// Json -> Instance
		entt::meta_any parseClassFromJson(const std::string& className,const nlohmann::json& inJson);
		void parseMemberFromJson(const nlohmann::json& inJson);
	};
}