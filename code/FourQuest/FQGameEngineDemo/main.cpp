#include <iostream>

#include "../FQReflect/FQReflect.h"


struct MyStruct
{
	int id = 1000;
	float hp = 40.f;
};

FQ_REGISTRATION
{
	entt::meta<MyStruct>()
		.type(entt::hashed_string("MyStruct"))
		.data<&MyStruct::id>(entt::hashed_string("MyStruct"))
		.prop(fq::reflect::tag::name, "id")
		.data<&MyStruct::hp>(entt::hashed_string("hp"))
		.prop(fq::reflect::tag::name, "hp");
}

using json = nlohmann::json;

int main()
{
	MyStruct st;

	entt::meta_any any = st;

	fq::reflect::Converter c;

	c.SerializeClass("dd", any);

	return 0;
}