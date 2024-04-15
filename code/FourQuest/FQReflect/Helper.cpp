#include "Helper.h"

#include <cassert>

#include "prop.h"

std::string fq::reflect::GetName(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::prop::name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetName(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::prop::name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetLabel(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::prop::label);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetComment(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::prop::comment);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}
