#include "Component.h"
#include "Scene.h"

#include "Transform.h"

FQ_REGISTRATION
{
	entt::meta<fq::game_module::Component>()
		.type(entt::hashed_string("Component"));
}

fq::game_module::Component::~Component()
{

}

fq::game_module::Scene* fq::game_module::Component::GetScene() const
{
	if (!mGameObject) return nullptr;

	return mGameObject->GetScene();
}

fq::game_module::Component::Component()
	:mGameObject(nullptr)
	, mbIsToBeRemoved(false)
{}

fq::game_module::Component::Component(const Component& other)
	:mbIsToBeRemoved(other.mbIsToBeRemoved)
	, mGameObject(nullptr)
{}

fq::game_module::Component& fq::game_module::Component::operator=(const Component& other)
{
	mbIsToBeRemoved = other.mbIsToBeRemoved;
	mGameObject = nullptr;

	return *this;
}

fq::game_module::Transform* fq::game_module::Component::GetTransform()
{
	return GetComponent<fq::game_module::Transform>();
}

