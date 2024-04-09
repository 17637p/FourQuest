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

		/// Instance -> Json 

		/// <summary>
		/// class instance를 json으로 파싱
		/// </summary>
		/// <param name="object">파싱하는 any객체</param>
		/// <param name="outJson">저장 json객체</param>
		/// <param name="memberClass">class의 멤버변수인경우 변수명</param>
		void ParseClassToJson(const entt::meta_any& object, nlohmann::json& outJson, const std::string& memberClass = "");
		 
		void ParseMemberToJson(const entt::meta_data& metaData, const entt::meta_any& object, nlohmann::json& outJson);
		
		void ParseSequenceContainerToJson(const entt::meta_any& element, nlohmann::json& arrayJson);

		void ParseAssociativeContainer(const entt::meta_any& key, const entt::meta_any& value, nlohmann::json& outJson);

		/// Json -> Instance
		
		entt::meta_any ParseClassFromJson(const std::string& className,const nlohmann::json& inJson);
		
		entt::meta_any ParseMemberFromJson(const nlohmann::json& inJson, const entt::meta_type& metaType);

		std::string ConvertString(const entt::meta_any& any);
	};
}