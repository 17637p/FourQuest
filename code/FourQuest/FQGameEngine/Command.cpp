#include "Command.h"

void fq::game_engine::CreateObject::Excute()
{
	mScene->AddGameObject(mGameObject);
}

void fq::game_engine::CreateObject::Undo()
{
	mScene->DestroyGameObject(mGameObject.get());
}

fq::game_engine::CreateObject::CreateObject(fq::game_module::Scene* scene
	, std::shared_ptr<fq::game_module::GameObject> object)
	:mScene(scene)
	,mGameObject(object)
{}

fq::game_engine::CreateObject::~CreateObject()
{

}
