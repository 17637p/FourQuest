#include <iostream>

#include "../FQReflect/FQReflect.h"

enum class ETag
{
	Component,
	Entity,
	Node,
};

struct Component
{
	int id = 500;
	float power = 101.010f;
};

struct MyStruct
{
	MyStruct() { std::cout << "생성자 호출\n"; }
	~MyStruct() { std::cout << "소멸자 호출\n"; }

	int id = 1000;
	float hp = 40.f;
	std::string path = "\\reseource\\ent.txt";
	ETag tag = ETag::Component;
	std::wstring koreanName = L"이기태";
	Component comp;
};

FQ_REGISTRATION
{
	entt::meta<ETag>()
		.data<ETag::Component>(entt::hashed_string("Component"))
		.data<ETag::Entity>(entt::hashed_string("Entity"))
		.data<ETag::Node>(entt::hashed_string("Node"));

	entt::meta<Component>()
		.type(entt::hashed_string("Component"))
		.data<&Component::id>(entt::hashed_string("id"))
		.prop(fq::reflect::tag::name, "id")
		.data<&Component::power>(entt::hashed_string("power"))
		.prop(fq::reflect::tag::name, "power");

	entt::meta<MyStruct>()
		.type(entt::hashed_string("MyStruct"))
		.data<&MyStruct::id>(entt::hashed_string("id"))
		.prop(fq::reflect::tag::name, "id")
		.data<&MyStruct::hp>(entt::hashed_string("hp"))
		.prop(fq::reflect::tag::name, "hp")
		.data<&MyStruct::path>(entt::hashed_string("path"))
		.prop(fq::reflect::tag::name, "path")
		.data<&MyStruct::tag>(entt::hashed_string("tag"))
		.prop(fq::reflect::tag::name, "tag")
		.data<&MyStruct::comp>(entt::hashed_string("comp"))
		.prop(fq::reflect::tag::name, "comp")
		.data<&MyStruct::koreanName>(entt::hashed_string("koreanName"))
		.prop(fq::reflect::tag::name, "koreanName");
}

using json = nlohmann::json;

int main()
{
	fq::reflect::Converter c;

	auto path = std::filesystem::current_path();

	path =  fq::path::CreateFilePath(path, L"new", L"json");

	MyStruct st;
	c.SerializeClass (path, st);

	c.DeserializeClass(path);


	return 0;
}