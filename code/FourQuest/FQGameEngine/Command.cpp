#include "Command.h"
#include <queue>
#include <spdlog/spdlog.h>

#include "EditorEvent.h"

void fq::game_engine::AddObjectCommand::Excute()
{
	mScene->AddGameObject(mGameObject);
}

void fq::game_engine::AddObjectCommand::Undo()
{
	mScene->DestroyGameObject(mGameObject.get());
}

fq::game_engine::AddObjectCommand::AddObjectCommand(fq::game_module::Scene* scene
	, std::shared_ptr<fq::game_module::GameObject> object)
	:mScene(scene)
	, mGameObject(object)
{
	// 메모리 해제를 막기위해서 자식들을 가진다.
	std::queue<fq::game_module::GameObject*> q;

	q.push(object.get());

	while (!q.empty())
	{
		auto tmp = q.front();
		q.pop();

		for (auto child : tmp->GetChildren())
		{
			mChildren.push_back(child->shared_from_this());
			q.push(child);
		}
	}
}

fq::game_engine::AddObjectCommand::~AddObjectCommand()
{}


void fq::game_engine::DestroyObjectCommand::Excute()
{
	mScene->DestroyGameObject(mGameObject.get());
}

void fq::game_engine::DestroyObjectCommand::Undo()
{
	mScene->AddGameObject(mGameObject);

	if (mParent != nullptr)
	{
		auto parentT = mParent->GetComponent<fq::game_module::Transform>();
		mGameObject->GetComponent<fq::game_module::Transform>()->SetParent(parentT);
	}
}

fq::game_engine::DestroyObjectCommand::DestroyObjectCommand(fq::game_module::Scene* scene
	, std::shared_ptr<fq::game_module::GameObject> object)
	:mScene(scene)
	, mGameObject(object)
	, mParent(nullptr)
{
	// 메모리 해제를 막기위해서 자식들을 가진다.
	std::queue<fq::game_module::GameObject*> q;

	q.push(object.get());

	while (!q.empty())
	{
		auto tmp = q.front();
		q.pop();

		for (auto child : tmp->GetChildren())
		{ 
			mChildren.push_back(child->shared_from_this());
			q.push(child);
		}
	}

	if (object->HasParent())
	{
		mParent = object->GetParent()->shared_from_this();
	}
}

void fq::game_engine::SelectObjectCommand::Excute()
{
	mEventManager->FireEvent<editor_event::SelectObject>({ mCurrentSelect });
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
