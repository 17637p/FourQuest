#include "Helper.h"

#include <cassert>

#include "Tag.h"

std::string fq::reflect::GetName(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::tag::name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetName(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::tag::name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetLabel(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::tag::label);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetComment(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::tag::comment);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}
