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
		/// Instance -> Json 

		/// <summary>
		/// class instance�� json���� �Ľ�
		/// </summary>
		/// <param name="object">�Ľ��ϴ� any��ü</param>
		/// <param name="outJson">���� json��ü</param>
		/// <param name="memberClass">class�� ��������ΰ�� ������</param>
		void parseClassToJson(const entt::meta_any& object, nlohmann::json& outJson, const std::string& memberClass = "");
		 
		void parseMemberToJson(const entt::meta_data& metaData, const entt::meta_any& object, nlohmann::json& outJson);
		
		void parseSequenceContainerToJson(const entt::meta_any& element, nlohmann::json& arrayJson);

		void parseAssociativeContainer(const entt::meta_any& key, const entt::meta_any& value, nlohmann::json& outJson);

		/// Json -> Instance
		
		entt::meta_any parseClassFromJson(const std::string& className,const nlohmann::json& inJson);
		
		entt::meta_any parseMemberFromJson(const nlohmann::json& inJson, const entt::meta_type& metaType);

		std::string convertString(const entt::meta_any& any);
	};
}