#include "GameObject.h"
#include "Component.h"
#include "Transform.h"

FQ_REGISTRATION
{
	entt::meta<fq::game_module::GameObject>()
		.type(entt::hashed_string("GameObject"))
		.prop(fq::reflect::prop::name, "GameObject")
		.data<&fq::game_module::GameObject::mID>(entt::hashed_string("mID"))
		.prop(fq::reflect::prop::name, "mID")
		.data<&fq::game_module::GameObject::mName>(entt::hashed_string("mName"))
		.prop(fq::reflect::prop::name, "mName")
		.data<&fq::game_module::GameObject::SetTag ,&fq::game_module::GameObject::GetTag>(entt::hashed_string("mTag"))
		.prop(fq::reflect::prop::name, "mTag");
}

fq::game_module::GameObject::GameObject()
	:mID(LastID++)
	, mName("GameObject")
	, mTag(Tag::Player)
	, mComponents{}
	, mScene(nullptr)
	, mbIsToBeDestroyed(false)
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
	mbIsToBeDestroyed = true;

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
	Transform* parentT = GetComponent<Transform>()->GetParentTransform();

	if (parentT)
	{
		return parentT->GetGameObject();
	}

	return  nullptr;
}

std::vector<fq::game_module::GameObject*> fq::game_module::GameObject::GetChildren()
{
	assert(HasComponent<Transform>());

	const auto& childrenTransform = GetComponent<Transform>()->GetChildren();

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
	// 하이픈 (-)을 언더스코어(_)로 치환
	// 프리팹을 저장할때 "부모이름-자식이름" 규칙으로 저장하기때문입니다.
	std::replace(name.begin(), name.end(), '-', '_');

	mName = std::move(name);
}

void fq::game_module::GameObject::AddComponent(entt::meta_any any)
{
	entt::meta_type type = any.type();
	entt::meta_type componentType = entt::resolve<Component>();

	assert(type.can_cast(componentType));

	const Component* component = any.try_cast<Component>();

	assert(component);

	Component* clone = component->Clone(nullptr);

	// 기존에 있던 컴포넌트는 제거합니다.
	auto iter = mComponents.find(type.id());
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}

	clone->SetGameObject(this);
	mComponents.insert({ type.id(), std::unique_ptr<Component>(clone)});
}

void fq::game_module::GameObject::DestroyAllComponent()
{
	mComponents.clear();
}

void fq::game_module::GameObject::DestroyComponent(entt::id_type id)
{
	auto iter = mComponents.find(id);

	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}
}

entt::meta_handle fq::game_module::GameObject::GetHandle()
{
	return *this;
}

