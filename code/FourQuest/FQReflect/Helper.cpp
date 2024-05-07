#include "Helper.h"

#include <cassert>

#include "prop.h"

std::string fq::reflect::GetName(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::prop::Name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetName(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::prop::Name);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetLabel(const entt::meta_type& metaType)
{
	auto prop = metaType.prop(fq::reflect::prop::Label);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

std::string fq::reflect::GetComment(const entt::meta_data& metaData)
{
	auto prop = metaData.prop(fq::reflect::prop::Comment);

	if (!prop)
	{
		return {};
	}
	return prop.value().cast<const char*>();
}

bool fq::reflect::IsReadOnly(const entt::meta_data& data)
{
	return data.prop(fq::reflect::prop::ReadOnly) ? true : false;
}
