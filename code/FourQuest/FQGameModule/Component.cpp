#include "Component.h"
#include "Scene.h"


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
	,mbIsToBeRemoved(false)
{}

