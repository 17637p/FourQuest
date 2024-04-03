#include <iostream>

#include "../FQReflect/FQReflect.h"


struct Component
{
	int id = 500;
	float power = 101.010f;
};

struct MyStruct
{

	std::map<int, int> i;
};

FQ_REGISTRATION
{
	entt::meta<Component>()
		.type(entt::hashed_string("Component"))
		.data<&Component::id>(entt::hashed_string("id"))
		.prop(fq::reflect::tag::name, "id")
		.data<&Component::power>(entt::hashed_string("power"))
		.prop(fq::reflect::tag::name, "power");

	entt::meta<MyStruct>()
		.type(entt::hashed_string("MyStruct"))
		.data<&MyStruct::i>(entt::hashed_string("comp"))
		.prop(fq::reflect::tag::name, "comp");
}

using json = nlohmann::json;

int main()
{
	fq::reflect::Converter c;

	auto path = std::filesystem::current_path();
	 
	path = fq::path::CreateFilePath(path, L"new", L"json");
	
	MyStruct s;

	for (int i = 0; i < 120; ++i)
	{
		s.i.insert({ i,i });
	}

	c.SerializeClass(path, s);

	auto s2 =c.DeserializeClass(path);

	MyStruct s3 = s2.cast<MyStruct>();


	return 0;
}