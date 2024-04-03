#pragma once

#include <filesystem>

#include "entt.hpp"
#include "json.hpp"
#include "json_fwd.hpp"

namespace fq::reflect
{
	/// <summary>
	/// ����ȭ Ŭ����
	/// </summary>
	class Converter
	{
	public:
		/// <summary>
		/// class ��ü�� json ������ ���Ϸ� ����ȭ
		/// </summary>
		/// <param name="path">���� ���</param>
		/// <param name="data">any ��ü�� ���� class</param>
		void SerializeClass(const std::filesystem::path& path, const entt::meta_any& object);

		/// <summary>
		/// json ������ ������ any ��ü�� ������ȭ
		/// </summary>
		/// <param name="path">���� ���� ���</param>
		/// <returns>������ȭ�� any ��ü</returns>
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