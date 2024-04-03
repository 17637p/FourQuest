#include "Converter.h"

#include <assert.h>
#include <fstream>
#include <iostream>

#include "Tag.h"

using json = nlohmann::json;

nlohmann::json fq::reflect::Converter::parseClass(const entt::meta_any& object)
{
	json output;
	const entt::meta_type& metaType = object.type();

	// member parsing
	for (const auto& member : metaType.data())
	{
		parseMember(member.second);
	}

	return output;
}

nlohmann::json fq::reflect::Converter::parseMember(const entt::meta_data& metaData)
{
	const entt::meta_any& metaName = metaData.prop(tag::name).value();
	const char* name = metaName.cast<const char*>();

	std::cout << name << '\n';

	return json();
}


void fq::reflect::Converter::SerializeClass(const std::filesystem::path& path, const entt::meta_any& object)
{
	assert(object.type().is_class());

	json j = parseClass(object);

	std::cout <<  "\nresult\n" << j;
}

entt::meta_any fq::reflect::Converter::DeserializeClass(const std::filesystem::path& path)
{
	return entt::meta_any();
}


