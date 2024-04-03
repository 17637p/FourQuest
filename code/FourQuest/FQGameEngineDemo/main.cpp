#include <iostream>

#include "../FQReflect/FQReflect.h"


struct Component
{
	int id = 500;
	float power = 101.010f;
};

struct MyStruct
{
	MyStruct() { std::cout << "생성자 호출\n"; }
	~MyStruct() { std::cout << "소멸자 호출\n"; }

	Component* comp;
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
		.data<&MyStruct::comp>(entt::hashed_string("comp"))
		.prop(fq::reflect::tag::name, "comp");
}

using json = nlohmann::json;

int main()
{
	fq::reflect::Converter c;

	auto path = std::filesystem::current_path();

	path = fq::path::CreateFilePath(path, L"new", L"json");

	entt::meta_any parseData = c.DeserializeClass(path);
	MyStruct st3 = parseData.cast<MyStruct>();

	MyStruct st1;
	c.SerializeClass(path, st1);

	return 0;
}