#include "Hierarchy.h"

#include <imgui.h>

#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "imgui_stdlib.h"

fq::game_engine::Hierarchy::Hierarchy()
	:mGameProcess(nullptr)
	,mEditorProcess(nullptr)
	,mScene(nullptr)
{}

fq::game_engine::Hierarchy::~Hierarchy()
{}


void fq::game_engine::Hierarchy::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	mScene = mGameProcess->mSceneManager->GetCurrentScene();
}

void fq::game_engine::Hierarchy::Render()
{
	if (ImGui::Begin("Hierarchy"))
	{
		BeginPopupContextWindow();
		BeginSearchBar();
		BeginGameObjectBar();
		ImGui::End();
	}
}

void fq::game_engine::Hierarchy::BeginPopupContextWindow()
{
	if (ImGui::BeginPopupContextWindow())
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mScene->AddGameObject(std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::BeginSearchBar()
{
	ImGui::InputText("Search", &mSearchString);
	ImGui::Separator();
}

void fq::game_engine::Hierarchy::BeginGameObjectBar()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (const auto& object : mScene->GetObjectView(true))
	{
		ImGui::Text(object.GetName().c_str());
	}
}
