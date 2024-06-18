#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "Scene.h"
#include "EventManager.h"
#include "Event.h"
#include "SceneHelper.h"

fq::game_module::GameObject::GameObject()
	:mID(LastID++)
	, mName("GameObject")
	, mTag(ETag::Untagged)
	, mComponents{}
	, mScene(nullptr)
	, mbIsDestroyed(false)
{
	AddComponent<Transform>();
}

fq::game_module::GameObject::~GameObject()
{}

fq::game_module::GameObject::GameObject(const GameObject& other)
	:mID(LastID++)
	, mName(other.mName)
	, mTag(other.mTag)
	, mScene(other.mScene)
	, mbIsDestroyed(other.mbIsDestroyed)
{
	// 컴포넌트 복사
	for (const auto& [id, component] : other.GetComponentContainer())
	{
		std::shared_ptr<Component> cloneComponent(component->Clone());
		cloneComponent->SetGameObject(this);
		mComponents.insert({ id,cloneComponent });
	}
}

fq::game_module::GameObject& fq::game_module::GameObject::operator=(const GameObject& other)
{
	this->mName = other.mName;
	this->mTag = other.mTag;
	this->mScene = other.mScene;
	this->mbIsDestroyed = other.mbIsDestroyed;

	// 중복하지 않는 컴포넌트는 삭제합니다 
	for (auto iter = mComponents.begin(); iter != mComponents.end();)
	{
		if (other.mComponents.find(iter->first) == other.mComponents.end())
		{
			iter = mComponents.erase(iter);
		}
		else
		{
			++iter;
		}
	}

	// 컴포넌트 복사
	for (const auto& [id, component] : other.mComponents)
	{
		auto iter = mComponents.find(id);

		if (iter == mComponents.end())
		{
			// 새로 생성
			std::shared_ptr<Component> cloneComponent(component->Clone());
			mComponents.insert({ id,cloneComponent });
			cloneComponent->SetGameObject(this);
		}
		else
		{
			// 덮어씌우기
			component->Clone(iter->second);
			component->SetGameObject(this);
		}
	}

	return *this;
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
	mbIsDestroyed = true;

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

	Component* component = any.try_cast<Component>();

	assert(component);

	auto clone = component->Clone(nullptr);

	// 기존에 있던 컴포넌트는 제거합니다.
	auto iter = mComponents.find(type.id());
	if (iter != mComponents.end())
	{
		mComponents.erase(iter);
	}

	clone->SetGameObject(this);
	mComponents.insert({ type.id(), clone });

	if (GetScene())
	{
		GetScene()->GetEventManager()
			->FireEvent<fq::event::AddComponent>({ iter->first,iter->second.get() });
	}
}

void fq::game_module::GameObject::AddComponent(entt::id_type id, std::shared_ptr<Component> component)
{
	auto iter = mComponents.find(id);
	assert(iter == mComponents.end());

	component->mbIsToBeRemoved = false;
	component->SetGameObject(this);
	mComponents.insert({ id, component });

	iter = mComponents.find(id);

	if (GetScene())
	{
		GetScene()->GetEventManager()
			->FireEvent<fq::event::AddComponent>({ iter->first,iter->second.get() });
	}
}

void fq::game_module::GameObject::RemoveAllComponent()
{
	mComponents.clear();
}

void fq::game_module::GameObject::RemoveComponent(entt::id_type id)
{
	auto iter = mComponents.find(id);

	if (iter != mComponents.end()
		&& !iter->second->mbIsToBeRemoved)
	{
		iter->second->mbIsToBeRemoved = true;

		if (GetScene())
		{
			GetScene()->GetEventManager()
				->FireEvent<fq::event::RemoveComponent>({ iter->first,iter->second.get() });
		}
	}
}

entt::meta_handle fq::game_module::GameObject::GetHandle()
{
	return *this;
}

fq::game_module::Component* fq::game_module::GameObject::GetComponent(entt::id_type id)
{
	auto iter = mComponents.find(id);

	if (iter == mComponents.end())
	{
		return nullptr;
	}

	return iter->second.get();
}

void fq::game_module::GameObject::CleanUpComponent()
{
	for (auto iter = mComponents.begin(); iter != mComponents.end(); )
	{
		if (iter->second->mbIsToBeRemoved)
		{
			iter = mComponents.erase(iter);
		}
		else
		{
			++iter;
		}
	}

}

bool fq::game_module::GameObject::HasParent()
{
	return GetParent() != nullptr;
}

void fq::game_module::GameObject::OnCollisionEnter(const Collision& collision)
{
	for ( auto&  [id,component] : mComponents)
	{
		component->OnCollisionEnter(collision);
	}
}

void fq::game_module::GameObject::OnCollisionExit(const Collision& collision)
{
	for (auto& [id, component] : mComponents)
	{
		component->OnCollisionExit(collision);
	}
}

void fq::game_module::GameObject::OnCollisionStay(const Collision& collision)
{
	for (auto& [id, component] : mComponents)
	{
		component->OnCollisionStay(collision);
	}
}

void fq::game_module::GameObject::OnTriggerEnter(const Collision& collision)
{
	for (auto& [id, component] : mComponents)
	{
		component->OnTriggerEnter(collision);
	}
}

void fq::game_module::GameObject::OnTriggerStay(const Collision& collision)
{
	for (auto& [id, component] : mComponents)
	{
		component->OnTriggerStay(collision);
	}
}

void fq::game_module::GameObject::OnTriggerExit(const Collision& collision)
{
	for (auto& [id, component] : mComponents)
	{
		component->OnTriggerExit(collision);
	}
}

fq::game_module::GameObject* fq::game_module::GameObject::GetRootObject()
{
	if (!HasParent())
		return this;

	GameObject* mRoot = nullptr;

	GameObject* child = this;
	do 
	{
		mRoot = child;
		child = child->GetParent();
	} while (child != nullptr);

	return mRoot;
}

