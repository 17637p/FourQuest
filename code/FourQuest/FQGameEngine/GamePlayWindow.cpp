#include "GamePlayWindow.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../FQGameModule/InputManager.h"
#include "../FQGraphics/IFQGraphics.h"

#include "EditorProcess.h"
#include "GameProcess.h"
#include "EditorEvent.h"

fq::game_engine::GamePlayWindow::GamePlayWindow()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mMode(EditorMode::Edit)
	, mbIsPauseGame(false)
{}

fq::game_engine::GamePlayWindow::~GamePlayWindow()
{}

void fq::game_engine::GamePlayWindow::Render()
{
	if (ImGui::Begin("GamePlay", 0, ImGuiWindowFlags_MenuBar))
	{
		beginMenuBar_Control();
		beginImage_GameScreen();
	}
	ImGui::End();
}

void fq::game_engine::GamePlayWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

}

void fq::game_engine::GamePlayWindow::Finalize()
{

}

void fq::game_engine::GamePlayWindow::beginMenuBar_Control()
{
	if (ImGui::BeginMenuBar())
	{
		beginButton_Play();
		beginButton_Stop();

		ImGui::EndMenuBar();
	}
}

void fq::game_engine::GamePlayWindow::beginButton_Stop()
{
	if (mbIsPauseGame)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (ImGui::Button("Stop"))
		{
			mbIsPauseGame = false;
			mGameProcess->mTimeManager->SetTimeScale(1.);
		}
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Stop"))
		{
			mbIsPauseGame = true;
			mGameProcess->mTimeManager->SetTimeScale(0.);
		}
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Ctrl + Shift + P");
	}
}

void fq::game_engine::GamePlayWindow::beginButton_Play()
{
	float currsorX = (ImGui::GetWindowSize().x / 2.f) - ImGui::CalcTextSize("Play").x;

	ImGui::SetCursorPosX(currsorX);

	if (mMode == EditorMode::Play)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		if (ImGui::Button("Play"))
		{
			SetMode(EditorMode::Edit);
		}

		ImGui::PopStyleColor(2);
	}
	else if (mMode == EditorMode::Edit)
	{
		if (ImGui::Button("Play"))
		{
			SetMode(EditorMode::Play);
		}
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Ctrl + P");
	}
}

void fq::game_engine::GamePlayWindow::SetMode(EditorMode mode)
{
	if (mMode == mode)
	{
		return;
	}

	auto currentSceneName = mGameProcess->mSceneManager->
		GetCurrentScene()->GetSceneName();

	// Edit -> Play
	if (mode == EditorMode::Play)
	{
		// 현재 씬을 저장합니다 
		mEditorProcess->mMainMenuBar->SaveScene();

		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, true });
	}
	// Play -> Edit
	else if (mode == EditorMode::Edit)
	{
		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, false });
	}

	mGameProcess->mEventManager->FireEvent<editor_event::SelectObject>({ nullptr });

	mMode = mode;
}

void fq::game_engine::GamePlayWindow::ExcutShortcut()
{
	const auto& input = mEditorProcess->mInputManager;

	if (input->IsKeyState(Key::Ctrl, KeyState::Hold)
		&& input->IsKeyState(Key::P, KeyState::Tap))
	{
		if (input->IsKeyState(Key::LShift, KeyState::Hold))
		{
			if (mbIsPauseGame)
			{
				mGameProcess->mTimeManager->SetTimeScale(1.);
			}
			else
			{
				mGameProcess->mTimeManager->SetTimeScale(0.);
			}

			mbIsPauseGame = !mbIsPauseGame;
		}
		else
		{
			if (mMode == EditorMode::Edit)
			{
				SetMode(EditorMode::Play);
			}
			else
			{
				SetMode(EditorMode::Edit);
			}
		}
	}

}

void fq::game_engine::GamePlayWindow::beginImage_GameScreen()
{
	//auto size = ImGui::GetWindowSize();
	//mGameProcess->mGraphics->SetViewportSize(size.x, size.y);

	auto current = ImGui::GetCurrentContext();
	auto pos = current->CurrentWindow->Pos;
	auto size = current->CurrentWindow->Size;
	auto idealSize = ImVec2(size.x, size.y - 3);
	auto maxpos = ImVec2(pos.x + idealSize.x, pos.y + idealSize.y);
	auto borderSize = current->CurrentWindow->WindowBorderSize;

	ImGui::GetWindowDrawList()->AddImage(
		mGameProcess->mGraphics->GetSRV(),
		ImVec2(pos.x, pos.y + 22),
		ImVec2(maxpos.x, maxpos.y));
}
