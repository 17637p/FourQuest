#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

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
	,mName("GameObject")
	,mTag(Tag::Untagged)
	,mComponents{}
	,mScene(nullptr)
	,mbIsToBeDestroyed(false)
{
	AddComponent<Transform>();
}

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

void fq::game_module::GameObject::OnAwake()
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnAwake();
	}
}

void fq::game_module::GameObject::OnStart()
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnStart();
	}
}

void fq::game_module::GameObject::OnUpdate(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnUpdate(dt);
	}
}

void fq::game_module::GameObject::OnDestroy()
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnDestroy();
	}
}

void fq::game_module::GameObject::OnLateUpdate(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnLateUpdate(dt);
	}
}

void fq::game_module::GameObject::OnFixedUpdate(float dt)
{
	for (const auto& [key, component] : mComponents)
	{
		component->OnFixedUpdate(dt);
	}
}

fq::game_module::GameObject* fq::game_module::GameObject::GetParent()
{
	Transform* parentT =  GetComponent<Transform>()->GetParentTransform();

	if (parentT)
	{
		parentT->GetGameObject();
	}

	return  nullptr;
}

std::vector<fq::game_module::GameObject*> fq::game_module::GameObject::GetChildren()
{
	const auto& childrenTransform =  GetComponent<Transform>()->GetChildren();

	std::vector<GameObject*> children;

	children.resize(childrenTransform.size());

	for (UINT i = 0; i < childrenTransform.size(); ++i)
	{
		children[i] = childrenTransform[i]->GetGameObject();
	}

	return children;
}

void fq::game_module::GameObject::SetName(std::string name)
{
	mName = std::move(name);
}

