#include "Command.h"
#include "EditorEvent.h"

void fq::game_engine::CreateObjectCommand::Excute()
{
	mScene->AddGameObject(mGameObject);
}

void fq::game_engine::CreateObjectCommand::Undo()
{
	mScene->DestroyGameObject(mGameObject.get());
}

fq::game_engine::CreateObjectCommand::CreateObjectCommand(fq::game_module::Scene* scene
	, std::shared_ptr<fq::game_module::GameObject> object)
	:mScene(scene)
	,mGameObject(object)
{}

fq::game_engine::CreateObjectCommand::~CreateObjectCommand()
{}


void fq::game_engine::DestroyObjectCommand::Excute()
{
	mScene->DestroyGameObject(mGameObject.get());
}

void fq::game_engine::DestroyObjectCommand::Undo()
{
	mScene->AddGameObject(mGameObject);
}

fq::game_engine::DestroyObjectCommand::DestroyObjectCommand(fq::game_module::Scene* scene
	, std::shared_ptr<fq::game_module::GameObject> object)
	:mScene(scene)
	, mGameObject(object)
{}

void fq::game_engine::SelectObjectCommand::Excute()
{
	mEventManager->FireEvent<editor_event::SelectObject>({mCurrentSelect});
}

void fq::game_engine::SelectObjectCommand::Undo()
{
	mEventManager->FireEvent<editor_event::SelectObject>({ mPrevSelect });
}

fq::game_engine::SelectObjectCommand::SelectObjectCommand(fq::game_module::EventManager* eventMgr
	, std::shared_ptr<fq::game_module::GameObject> current
	, std::shared_ptr<fq::game_module::GameObject> prev)
	:mEventManager(eventMgr)
	, mCurrentSelect(current)
	, mPrevSelect(prev)
{}
