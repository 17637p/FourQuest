#include "GameModulePCH.h"
#include "GameObject.h"

FQ_REGISTRATION
{
	entt::meta<fq::game_module::GameObject>()
		.type(entt::hashed_string("GameObject"))
		.data<&fq::game_module::GameObject::mID>(entt::hashed_string("mID")).prop(fq::reflect::tag::name, "mID")
		.data<&fq::game_module::GameObject::mName>(entt::hashed_string("mName")).prop(fq::reflect::tag::name, "mName")
		.data<&fq::game_module::GameObject::mComponents>(entt::hashed_string("mComponents")).prop(fq::reflect::tag::name, "mComponents")
		.data<&fq::game_module::GameObject::mTag>(entt::hashed_string("mTag")).prop(fq::reflect::tag::name, "mTag");
}

fq::game_module::GameObject::GameObject()
{

}

fq::game_module::GameObject::GameObject(const GameObject& other)
{

}

fq::game_module::GameObject::~GameObject()
{

}

fq::game_module::GameObject& fq::game_module::GameObject::operator=(const GameObject& other)
{

}

void fq::game_module::GameObject::Initialize()
{

}

void fq::game_module::GameObject::Start()
{

}

void fq::game_module::GameObject::Update(float dt)
{

}

void fq::game_module::GameObject::Finalize()
{

}
