#include "Converter.h"

#include <assert.h>
#include <fstream>
#include <iostream>
#include <string>

#include "../FQCommon/FQPath.h"
#include "prop.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void fq::reflect::Converter::ParseClassToJson(const entt::meta_any& object
	, nlohmann::json& outJson
	, const std::string& memberClass /*= ""*/)
{
	json memberJson;
	const entt::meta_type& metaType = object.type();

	// member parsing
	for (auto&& [id, member] : metaType.data())
	{
		ParseMemberToJson(member, object, memberJson);
	}

	if (memberClass.empty())
	{
		std::string className = std::to_string(metaType.id());
		outJson[className] = memberJson;
	}
	else
	{
		outJson[memberClass] = memberJson;
	}
}


void fq::reflect::Converter::ParseMemberToJson(const entt::meta_data& metaData
	, const entt::meta_any& object
	, nlohmann::json& outJson)
{
	entt::meta_prop prop = metaData.prop(prop::Name);
	assert(prop && "멤버 변수의 name 설정이 없습니다.");

	const entt::meta_any& metaName = prop.value();
	const char* name = metaName.cast<const char*>();

	entt::meta_type metaType = metaData.type();
	entt::meta_any  anyValue = metaData.get(object);

	// bool
	if (metaType == entt::resolve<bool>())
	{
		bool val = anyValue.cast<bool>();
		outJson[name] = val;
	}
	// int
	else if (metaType == entt::resolve<int>())
	{
		int val = anyValue.cast<int>();
		outJson[name] = val;
	}
	// unsigned int
	else if (metaType == entt::resolve<unsigned int>())
	{
		unsigned int val = anyValue.cast<unsigned int>();
		outJson[name] = val;
	}
	// long long
	else if (metaType == entt::resolve<long long>())
	{
		long long val = anyValue.cast<long long>();
		outJson[name] = val;
	}
	// unsigned long long
	else if (metaType == entt::resolve<unsigned long long>())
	{
		unsigned long long val = anyValue.cast<unsigned long long>();
		outJson[name] = val;
	}
	// float 
	else if (metaType == entt::resolve<float>())
	{
		float val = anyValue.cast<float>();
		outJson[name] = val;
	}
	else if (metaType == entt::resolve<double>())
	{
		double val = anyValue.cast<double>();
		outJson[name] = val;
	}
	// std::string
	else if (metaType == entt::resolve<std::string>())
	{
		std::string val = anyValue.cast<std::string>();

		if (metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetRelativePath(val).string();
		}

		outJson[name] = val;
	}
	// std::wstring
	else if (metaType == entt::resolve<std::wstring>())
	{
		std::wstring val = anyValue.cast<std::wstring>();
		if (metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetRelativePath(val);
		}

		outJson[name] = val;
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		const char* val = anyValue.cast<const char*>();
		if (metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetRelativePath(val).string().c_str();
		}

		outJson[name] = val;
	}
	// const wchar_t*
	else if (metaType == entt::resolve<const wchar_t*>())
	{
		const wchar_t* val = anyValue.cast<const wchar_t*>();
		if (metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetRelativePath(val).c_str();
		}

		outJson[name] = val;
	}
	// enum 
	else if (metaType.is_enum())
	{
		bool check = anyValue.allow_cast<int>();
		assert(check);
		int val = anyValue.cast<int>();
		outJson[name] = val;
	}
	// 연속 컨테이너 ex) std::vector, std::array, std::deque, std::list 
	else if (metaType.is_sequence_container())
	{
		auto view = anyValue.as_sequence_container();

		json arrayJson;

		for (const entt::meta_any& element : view)
		{
			entt::meta_type elementType = element.type();
			ParseSequenceContainerToJson(element, arrayJson);
		}

		outJson[name] = arrayJson;
	}
	// 연관 컨테이너 ex) std::map, std::set, std::unordered_map
	else if (metaType.is_associative_container())
	{
		auto view = anyValue.as_associative_container();

		json arrayJson;

		for (const auto& [key, value] : view)
		{
			ParseAssociativeContainer(key, value, arrayJson);
		}

		outJson[name] = arrayJson;
	}
	// class
	else if (metaType.is_class())
	{
		ParseClassToJson(metaData.get(object), outJson, name);
	}
	else
	{
		assert(!"member parsing error");
	}
}

void fq::reflect::Converter::SerializeClass(const std::filesystem::path& path, const entt::meta_any& object)
{
	assert(object.type().is_class());
	json j;

	ParseClassToJson(object, j);

	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << j;
		output.close();
	}
}

entt::meta_any fq::reflect::Converter::DeserializeClass(const std::filesystem::path& path)
{
	assert(fs::exists(path));
	assert(path.extension() == ".json");

	std::ifstream readFile(path);
	json readJson;
	if (readFile.is_open())
	{
		readFile >> readJson;
		readFile.close();
	}

	// json을 읽어서 output 객체 생성
	entt::meta_any output;
	for (const auto& element : readJson.items())
	{
		output = ParseClassFromJson(element.key(), element.value());
	}

	return output;
}

entt::meta_any fq::reflect::Converter::ParseClassFromJson(const std::string& classID, const nlohmann::json& inJson)
{
	// class meta_type 생성
	entt::id_type id = std::stoul(classID);
	entt::meta_type classMetaType = entt::resolve(id);
	assert(classMetaType);

	// class instance 생성
	entt::meta_any instance = classMetaType.construct();

	// class member 생성
	for (const auto& element : inJson.items())
	{
		const std::string& memberName = element.key();
		entt::id_type id = entt::hashed_string(memberName.c_str());
		entt::meta_data memberMetaData = classMetaType.data(id);

		if (memberMetaData)
		{
			entt::meta_any val = ParseMemberFromJson(element.value(), memberMetaData.type(),memberMetaData);
			memberMetaData.set(instance, val);
		}
		else
		{
			// 리플렉션한 데이터가 변경된 경우
		}
	}

	return instance;
}

entt::meta_any fq::reflect::Converter::ParseMemberFromJson(const nlohmann::json& inJson
	, const entt::meta_type& metaType
	, const entt::meta_data& metaData)
{
	entt::meta_any output;

	// bool
	if (metaType == entt::resolve<bool>())
	{
		bool val = inJson.get<bool>();
		output = val;
	}
	// int
	else if (metaType == entt::resolve<int>())
	{
		int val = inJson.get<int>();
		output = val;
	}
	// unsigned int
	else if (metaType == entt::resolve<unsigned int>())
	{
		unsigned int val = inJson.get<unsigned int>();
		output = val;
	}
	// long long
	else if (metaType == entt::resolve<long long>())
	{
		long long val = inJson.get<long long>();
		output = val;
	}
	// unsigned long long
	else if (metaType == entt::resolve<unsigned long long>())
	{
		unsigned long long val = inJson.get<unsigned long long>();
		output = val;
	}
	// float 
	else if (metaType == entt::resolve<float>())
	{
		float val = inJson.get<float>();
		output = val;
	}
	// double
	else if (metaType == entt::resolve<double>())
	{
		double val = inJson.get<double>();
		output = val;
	}
	// std::string
	else if (metaType == entt::resolve<std::string>())
	{
		std::string val = inJson.get<std::string>();

		if (metaData && metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetAbsolutePath(val).string();
		}

		output = val;
	}
	// std::wstring
	else if (metaType == entt::resolve<std::wstring>())
	{
		std::wstring val = inJson.get<std::wstring>();

		if (metaData && metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetAbsolutePath(val);
		}
		output = val;
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		std::string val = inJson.get<std::string>();

		if (metaData && metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetAbsolutePath(val).string();
		}

		output = val.c_str();
	}
	// const wchar_t*
	else if (metaType == entt::resolve<const wchar_t*>())
	{
		std::wstring val = inJson.get<std::wstring>();

		if (metaData && metaData.prop(fq::reflect::prop::RelativePath))
		{
			val = fq::path::GetAbsolutePath(val);
		}
		output = val.c_str();
	}
	// enum 
	else if (metaType.is_enum())
	{
		int val = inJson.get<int>();
		output = val;
	}
	// 연속 컨테이너 ex) std::vector, std::array, std::deque, std::list 
	else if (metaType.is_sequence_container())
	{
		output = metaType.construct();

		auto view = output.as_sequence_container();
		entt::meta_type itemMetaType = view.value_type();

		for (auto& item : inJson)
		{
			entt::meta_any itemInstance = ParseMemberFromJson(item, itemMetaType, {});
			view.insert(view.end(), itemInstance);
		}
	}
	// 연관 컨테이너 ex) std::map, std::set, std::unordered_map
	else if (metaType.is_associative_container())
	{
	}
	// class
	else if (metaType.is_class())
	{
		std::string classID = std::to_string(metaType.id());
		output = ParseClassFromJson(classID, inJson);
	}
	else
	{
		assert(!"member parsing error");
	}

	return output;
}


void fq::reflect::Converter::ParseSequenceContainerToJson(const entt::meta_any& element, nlohmann::json& arrayJson)
{
	entt::meta_type metaType = element.type();

	// bool
	if (metaType == entt::resolve<bool>())
	{
		bool val = element.cast<bool>();
		arrayJson.push_back(val);
	}
	// int
	else if (metaType == entt::resolve<int>())
	{
		int val = element.cast<int>();
		arrayJson.push_back(val);
	}

	// unsigned int
	else if (metaType == entt::resolve<unsigned int>())
	{
		unsigned int val = element.cast<unsigned int>();
		arrayJson.push_back(val);
	}
	// long long
	else if (metaType == entt::resolve<long long>())
	{
		long long val = element.cast<long long>();
		arrayJson.push_back(val);
	}
	// unsigned long long
	else if (metaType == entt::resolve<unsigned long long>())
	{
		unsigned long long val = element.cast<unsigned long long>();
		arrayJson.push_back(val);
	}
	// float 
	else if (metaType == entt::resolve<float>())
	{
		float val = element.cast<float>();
		arrayJson.push_back(val);
	}
	// double
	else if (metaType == entt::resolve<double>())
	{
		double val = element.cast<double>();
		arrayJson.push_back(val);
	}
	// std::string
	else if (metaType == entt::resolve<std::string>())
	{
		std::string val = element.cast<std::string>();
		arrayJson.push_back(val);
	}
	// std::wstring
	else if (metaType == entt::resolve<std::wstring>())
	{
		std::wstring val = element.cast<std::wstring>();
		arrayJson.push_back(val);
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		const char* val = element.cast<const char*>();
		arrayJson.push_back(val);
	}
	// const wchar_t*
	else if (metaType == entt::resolve<const wchar_t*>())
	{
		const wchar_t* val = element.cast<const wchar_t*>();
		arrayJson.push_back(val);
	}
	// enum 
	else if (metaType.is_enum())
	{
		auto check = element.allow_cast<int>();
		assert(check);
		int val = element.cast<int>();
		arrayJson.push_back(val);
	}
	// class
	else if (metaType.is_class())
	{
		json clazz;
		ParseClassToJson(element, clazz);

		for (auto& item : clazz.items())
		{
			arrayJson.push_back(item.value());
		}
	}
	else
	{
		assert(!"member parsing error");
	}
}

void fq::reflect::Converter::ParseAssociativeContainer(const entt::meta_any& key
	, const entt::meta_any& value
	, nlohmann::json& outJson)
{
	outJson[ConvertString(key)] = ConvertString(value);
}

std::string fq::reflect::Converter::ConvertString(const entt::meta_any& any)
{
	std::string output;

	entt::meta_type metaType = any.type();
	if (metaType == entt::resolve<bool>())
	{
		bool val = any.cast<bool>();
		output = std::to_string(val);
	}
	// int
	else if (metaType == entt::resolve<int>())
	{
		int val = any.cast<int>();
		output = std::to_string(val);
	}
	// unsigned int
	else if (metaType == entt::resolve<unsigned int>())
	{
		unsigned int val = any.cast<unsigned int>();
		output = std::to_string(val);
	}
	// long long
	else if (metaType == entt::resolve<long long>())
	{
		long long val = any.cast<long long>();
		output = std::to_string(val);
	}
	// unsigned long long
	else if (metaType == entt::resolve<unsigned long long>())
	{
		unsigned long long val = any.cast<unsigned long long>();
		output = std::to_string(val);
	}
	// float 
	else if (metaType == entt::resolve<float>())
	{
		float val = any.cast<float>();
		output = std::to_string(val);
	}
	// double
	else if (metaType == entt::resolve<double>())
	{
		double val = any.cast<double>();
		output = std::to_string(val);
	}
	// std::string
	else if (metaType == entt::resolve<std::string>())
	{
		std::string val = any.cast<std::string>();
		output = val;
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		const char* val = any.cast<const char*>();
		output = val;
	}
	// enum 
	else if (metaType.is_enum())
	{
		int val = any.cast<int>();
		output = std::to_string(val);
	}

	return output;
}

