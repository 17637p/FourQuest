#include "GameObject.h"
#include "Component.h"

FQ_REGISTRATION
{
	entt::meta<fq::game_module::GameObject>()
		.type(entt::hashed_string("GameObject"))
		.data<&fq::game_module::GameObject::mID>(entt::hashed_string("mID")).prop(fq::reflect::tag::name, "mID") 
		.data<&fq::game_module::GameObject::mName>(entt::hashed_string("mName")).prop(fq::reflect::tag::name, "mName")
		.data<&fq::game_module::GameObject::mTag>(entt::hashed_string("mTag")).prop(fq::reflect::tag::name, "mTag");
}

fq::game_module::GameObject::GameObject()
	:mID(0)
	,mName()
	,mTag(Tag::Untagged)
	,mComponents{}
	,mScene(nullptr)
	,mbIsToBeDestroyed(false)
{}

fq::game_module::GameObject::~GameObject()
{}

fq::game_module::GameObject::GameObject(const GameObject& other)
{
	this->mID = other.mID;
	this->mName = other.mName;
	this->mTag = other.mTag;
	this->mScene = other.mScene;
	this->mbIsToBeDestroyed = other.mbIsToBeDestroyed;
}

fq::game_module::GameObject& fq::game_module::GameObject::operator=(const GameObject& other)
{
	this->mID = other.mID;
	this->mName = other.mName;
	this->mTag = other.mTag;
	this->mScene = other.mScene;

	return *this;
}

void fq::game_module::GameObject::Initialize()
{
	for (const auto& [key, component] : mComponents)
	{
		component->Initialize();
	}
}

void fq::game_module::GameObject::Start()
{
	for (const auto& [key, component] : mComponents)
	{
		component->Start();
	}
}

void fq::game_module::GameObject::Update(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->Update(dt);
	}
}

void fq::game_module::GameObject::Finalize()
{
	for (const auto& [key, component] : mComponents)
	{
		component->Finalize();
	}
}

void fq::game_module::GameObject::LateUpdate(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->LateUpdate(dt);
	}
}

void fq::game_module::GameObject::FixedUpdate(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->FixedUpdate(dt);
	}
}

