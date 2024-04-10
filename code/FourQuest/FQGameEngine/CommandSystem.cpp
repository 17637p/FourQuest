
#include "CommandSystem.h"
#include "ICommand.h"
#include "GameProcess.h"
#include "EditorProcess.h"

fq::game_engine::CommandSystem::CommandSystem()
	:mCommandList{}
	, mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mInputManager(nullptr)
	, mCommandOrder{}
{}

fq::game_engine::CommandSystem::~CommandSystem()
{
	Clear();
}


void fq::game_engine::CommandSystem::Clear()
{
	mCommandList.clear();
}

void fq::game_engine::CommandSystem::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mInputManager = mEditorProcess->mInputManager.get();

	// 빈 커맨드를 앞에 삽입
	mCommandList.push_front(std::make_unique<ICommand>());
	mCommandOrder = mCommandList.end();
}

void fq::game_engine::CommandSystem::excute()
{
	// 더이상 실행할 명령이 없는 경우
	if (mCommandOrder == mCommandList.end())
	{
		return;
	}

	(*mCommandOrder)->Excute();

	++mCommandOrder;
}

void fq::game_engine::CommandSystem::undo()
{
	if (mCommandOrder == mCommandList.begin()
		|| mCommandOrder == mCommandList.end())
	{
		return;
	}

	(*mCommandOrder)->Undo();

	--mCommandOrder;
}

void fq::game_engine::CommandSystem::Push(std::unique_ptr<ICommand> command)
{
	// 새로운 명령이 들어왔으므로 기존에 있던 명령을 지웁니다
	mCommandList.erase(mCommandOrder, mCommandList.end());

	// 새로운 명령을 추가합니다.
	mCommandOrder = mCommandList.insert(mCommandOrder, std::move(command));

	// 명령을 실행합니다
	(*mCommandOrder)->Excute();

	// 순서는 end입니다
	mCommandOrder = mCommandList.end();
}

void fq::game_engine::CommandSystem::Update()
{
	// Ctrl + Z
	if (mInputManager->IsKeyState(Key::Ctrl, KeyState::Hold)
		&& mInputManager->IsKeyState(Key::Z,KeyState::Tap))
	{
		undo();
	}

	// Ctrl + Y
	if (mInputManager->IsKeyState(Key::Ctrl, KeyState::Hold)
		&& mInputManager->IsKeyState(Key::Y, KeyState::Tap))
	{
		excute();
	}
}

