#include "Converter.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Tag.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void fq::reflect::Converter::parseClassToJson(const entt::meta_any& object, nlohmann::json& outJson, const std::string& memberClass /*= ""*/)
{
	json memberJson;
	const entt::meta_type& metaType = object.type();

	// member parsing
	for (auto&& [id, member] : metaType.data())
	{
		parseMemberToJson(member, object, memberJson);
	}

	if (memberClass.empty())
	{
		std::string className(metaType.info().name());

		// "class Example" -> "Example"
		className = className.substr(className.find(' ') + 1);

		outJson[className] = memberJson;
	}
	else
	{
		outJson[memberClass] = memberJson;
	}
}


void fq::reflect::Converter::parseMemberToJson(const entt::meta_data& metaData, const entt::meta_any& object, nlohmann::json& outJson)
{
	entt::meta_prop prop = metaData.prop(tag::name);
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
		outJson[name] = val;
	}
	// std::wstring
	else if (metaType == entt::resolve<std::wstring>())
	{
		std::wstring val = anyValue.cast<std::wstring>();
		outJson[name] = val;
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		const char* val = anyValue.cast<const char*>();
		outJson[name] = val;
	}
	// const wchar_t*
	else if (metaType == entt::resolve<const wchar_t*>())
	{
		const wchar_t* val = anyValue.cast<const wchar_t*>();
		outJson[name] = val;
	}
	// enum 
	else if (metaType.is_enum())
	{
		assert(anyValue.allow_cast<int>());
		int val = anyValue.cast<int>();
		outJson[name] = val;
	}
	// 연속 컨테이너 ex) std::vector, std::array, std::deque, std::list 
	else if (metaType.is_sequence_container())
	{
		auto view = anyValue.as_sequence_container();
	}
	// 연관 컨테이너 ex) std::map, std::set, std::unordered_map
	else if (metaType.is_associative_container())
	{
		auto view = anyValue.as_associative_container();
	}
	// class
	else if (metaType.is_class())
	{
		parseClassToJson(metaData.get(object), outJson, name);
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

	parseClassToJson(object, j);

	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << j;
		output.close();
	}
	std::cout << std::setw(4) << j;
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
		output = parseClassFromJson(element.key(), element.value());
	}

	return output;
}

entt::meta_any fq::reflect::Converter::parseClassFromJson(const std::string& className, const nlohmann::json& inJson)
{
	// class meta_type 생성
	entt::id_type id = entt::hashed_string(className.c_str()).value();
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
			entt::meta_any val = parseMemberFromJson(element.value(), memberMetaData);
			memberMetaData.set(instance, val);
		}
		else
		{
			// 리플렉션한 데이터가 변경된 경우

		}
	}

	return instance;
}

entt::meta_any fq::reflect::Converter::parseMemberFromJson(const nlohmann::json& inJson, const entt::meta_data& metaData)
{
	entt::meta_type metaType = metaData.type();
	entt::meta_any output;

	// bool
	if (metaType == entt::resolve<bool>())
	{
		std::cout << inJson << '\n';
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
		output = val;
	}
	// std::wstring
	else if (metaType == entt::resolve<std::wstring>())
	{
		std::wstring val = inJson.get<std::wstring>();
		output = val;
	}
	// const char*
	else if (metaType == entt::resolve<const char*>())
	{
		std::string val = inJson.get<std::string>();
		output = val.c_str();
	}
	// const wchar_t*
	else if (metaType == entt::resolve<const wchar_t*>())
	{
		std::wstring val = inJson.get<std::wstring>();
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
	}
	// 연관 컨테이너 ex) std::map, std::set, std::unordered_map
	else if (metaType.is_associative_container())
	{
	}
	// class
	else if (metaType.is_class())
	{
		std::string className{ metaType.info().name() };
		// "class Example" -> "Example"
		className = className.substr(className.find(' ') + 1);
		output = parseClassFromJson(className, inJson);
	}
	else
	{
		assert(!"member parsing error");
	}

	return output;
}


