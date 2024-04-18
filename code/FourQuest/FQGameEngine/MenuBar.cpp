#include "MenuBar.h"

#include <string>
#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"

fq::game_engine::MenuBar::MenuBar()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mbIsOpenCreateWindowPopup(false)
	, mCreateSceneName{ "4Quset" }
	, mCurrentSceneName{}
	, mSceneList{}
	, mOnLoadSceneHandler{}
{}

fq::game_engine::MenuBar::~MenuBar()
{

}

void fq::game_engine::MenuBar::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;



	// 이벤트 핸들 등록
	mGameProcess->mEventManager
		->RegisterHandle<fq::event::OnLoadScene>([this](const fq::event::OnLoadScene& event)
			{
				this->SetCurrentSceneName(event.sceneName);
			});
}

void fq::game_engine::MenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		beginMenu_File();
		beginText_FPS();
	}
	ImGui::EndMainMenuBar();

	beginOpenPopup_CreateScene();
}

void fq::game_engine::MenuBar::beginText_FPS()
{
	std::string fps = "FPS " + std::to_string(mGameProcess->mTimeManager->GetFPS());

	float cursorPosX = ImGui::GetWindowSize().x - ImGui::CalcTextSize(fps.c_str()).x;

	ImGui::SetCursorPosX(cursorPosX);
	ImGui::Text(fps.c_str());
}

void fq::game_engine::MenuBar::beginMenuItem_CreateScene()
{
	if (ImGui::MenuItem("Create Scene"))
	{
		mbIsOpenCreateWindowPopup = true;
	}
}

void fq::game_engine::MenuBar::beginMenuItem_LoadScene()
{
	if (ImGui::BeginMenu("Load Scene"))
	{
		updateSceneList();

		for (const auto& path : mSceneList)
		{
			std::string sceneName = path.filename().string();

			if (ImGui::MenuItem(sceneName.c_str()))
			{
				// Scene 변경 요청 
				mGameProcess->mEventManager
					->FireEvent<fq::event::RequestChangeScene>({ sceneName });
			}
		}

		ImGui::EndMenu();
	}
}

void fq::game_engine::MenuBar::beginMenuItem_SaveScene()
{
	if (ImGui::MenuItem("Save Scene"))
	{

	}
}

void fq::game_engine::MenuBar::beginMenu_File()
{
	if (ImGui::BeginMenu("File"))
	{
		beginMenuItem_CreateScene();
		beginMenuItem_LoadScene();
		beginMenuItem_SaveScene();

		ImGui::EndMenu();
	}
}

void fq::game_engine::MenuBar::beginOpenPopup_CreateScene()
{
	if (mbIsOpenCreateWindowPopup)
	{
		ImGui::OpenPopup("Create Scene");
		mbIsOpenCreateWindowPopup = false;
	}

	if (ImGui::BeginPopupModal("Create Scene"
		, nullptr
		, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Scene Name ##create scene popup winodw"
			, &mCreateSceneName);

		// 생성 
		if (ImGui::Button("Create"))
		{
			// 씬 중복을 확인 
			updateSceneList();

			createScene(mCreateSceneName);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// 취소
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::MenuBar::updateSceneList()
{
	auto scenePath = fq::path::GetScenePath();
	mSceneList = fq::path::GetFileList(scenePath);
}

void fq::game_engine::MenuBar::createScene(std::string sceneName)
{
	namespace fs = std::filesystem;

	// scene 폴더 생성
	PathType scenePath = fq::path::GetScenePath();
	scenePath /= sceneName;
	fs::create_directory(scenePath);

	// prefab 폴더 생성
	PathType prefabPath = scenePath;
	prefabPath /= "prefab";
	fs::create_directory(prefabPath);

	// ... etc 


	// Scene 변경 요청 
	mGameProcess->mEventManager
		->FireEvent<fq::event::RequestChangeScene>({ sceneName });
}

void fq::game_engine::MenuBar::SetCurrentSceneName(std::string sceneName)
{
	mCreateSceneName = std::move(sceneName);
}
