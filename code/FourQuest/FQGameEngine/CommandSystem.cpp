
#include "CommandSystem.h"

#include <spdlog/spdlog.h>

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
	ClearCommandList();
}

void fq::game_engine::CommandSystem::ClearCommandList()
{
	mCommandList.clear();
	mCommandOrder = mCommandList.end();
}

void fq::game_engine::CommandSystem::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mInputManager = mEditorProcess->mInputManager.get();

	mCommandOrder = mCommandList.end();

	// �̺�Ʈ �ڵ� ���
	mOnChangeSceneHandler = mGameProcess->mEventManager
		->RegisterHandle <fq::event::OnUnloadScene>(
		[this](fq::event::OnUnloadScene event) 
		{
			this->ClearCommandList();
		});
}

void fq::game_engine::CommandSystem::excute()
{
	// ���̻� ������ ����� ���� ���
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


void fq::game_engine::CommandSystem::ExcuteShortcut()
{
	// ���� �÷����߿��� Ŀ�ǵ� �Է��� ���� �ʴ´�
	if (mEditorProcess->mGamePlayWindow->GetMode() != EditorMode::Edit)
	{
		return;
	}

	if (!mInputManager->IsKeyState(EKey::LMouse, EKeyState::None))
	{
		return;
	}

	// Ctrl + Z
	if (mInputManager->IsKeyState(EKey::Ctrl, EKeyState::Hold)
		&& mInputManager->IsKeyState(EKey::Z,EKeyState::Tap))
	{
		undo();
	}
	// Ctrl + Y
	else if (mInputManager->IsKeyState(EKey::Ctrl, EKeyState::Hold)
		&& mInputManager->IsKeyState(EKey::Y, EKeyState::Tap))
	{
		excute();
	}
}


