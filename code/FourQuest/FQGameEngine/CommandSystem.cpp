
#include "CommandSystem.h"
#include "Command.h"
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

	// �� Ŀ�ǵ带 �տ� ����
	mCommandOrder = mCommandList.end();
}

void fq::game_engine::CommandSystem::excute()
{
	// ���̻� ������ ������ ���� ���
	if (mCommandOrder == mCommandList.end())
	{
		return;
	}

	(*mCommandOrder)->Excute();

	++mCommandOrder;
}

void fq::game_engine::CommandSystem::undo()
{
	if (mCommandOrder == mCommandList.begin())
	{
		return;
	}

	--mCommandOrder;

	(*mCommandOrder)->Undo();
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
