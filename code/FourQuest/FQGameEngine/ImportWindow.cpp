#include "ImportWindow.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include "GameProcess.h"
#include "../FQGameModule/SceneManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/Light.h"
#include "../FQGameModule/GameObject.h"
#include "EditorProcess.h"

fq::game_engine::ImportWindow::ImportWindow()
	:mGameProcess(nullptr),
	mEditorProcess(nullptr)
{
}

fq::game_engine::ImportWindow::~ImportWindow()
{

}

void fq::game_engine::ImportWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mScene = mGameProcess->mSceneManager->GetCurrentScene();
}

void fq::game_engine::ImportWindow::Render()
{
	if (!mbIsOpen) return;

	// 경로 받기
	if (ImGui::Begin("ImportWindow", &mbIsOpen))
	{
		std::string dirPath = mDirPath.string();
		ImGui::InputText("ImportDirectoryPath", &dirPath);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

			if (pathPayLoad)
			{
				std::filesystem::path* path
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (std::filesystem::exists(*path) && std::filesystem::is_directory(*path))
				{
					mDirPath = *path;
				}
			}
		}

		if (ImGui::Button("Import", ImVec2{ 133,25 }))
		{
			loadGameObjectsForUnityByDirectory();
		}
	}
	ImGui::End();
}

void fq::game_engine::ImportWindow::loadGameObjectsForUnityByDirectory()
{
	if (!std::filesystem::exists(mDirPath) || !std::filesystem::is_directory(mDirPath))
	{
		return;
	}

	std::vector<std::shared_ptr<fq::game_module::GameObject>> rootObjects;
	std::map<int, std::shared_ptr<fq::game_module::GameObject>> gameObjectsMap;

	for (const auto& dirIter : std::filesystem::directory_iterator{ mDirPath })
	{
		std::vector<fq::game_engine::GameObjectLoadInfo> gameObjectInfos = loadGameObjectInfosByJson(dirIter.path());

		for (const auto& gameObjectInfo : gameObjectInfos)
		{
			std::shared_ptr<fq::game_module::GameObject> gameObject = std::make_shared<fq::game_module::GameObject>();
			gameObject->SetName(gameObject->GetName() + std::to_string(gameObjectInfo.ID));
			// addComponent

			gameObjectsMap.insert({ gameObjectInfo.ID, std::move(gameObject) });
		}

		if (!gameObjectInfos.empty())
		{
			auto find = gameObjectsMap.find(gameObjectInfos[0].ID);

			if (find != gameObjectsMap.end())
			{
				rootObjects.push_back(find->second);
			}
		}
	}

	for (auto& gameObject : rootObjects)
	{
		mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene, gameObject);
	}
}

std::vector<fq::game_engine::GameObjectLoadInfo> fq::game_engine::ImportWindow::loadGameObjectInfosByJson(const std::filesystem::path& filePath)
{
	assert(std::filesystem::exists(filePath));

	std::ifstream readData(filePath);
	nlohmann::ordered_json gameObjectJson;

	if (readData.is_open())
	{
		readData >> gameObjectJson;
		readData.close();
	}
	else
		assert(!"파일 열기 실패");

	std::vector<GameObjectLoadInfo> objectInfos;


	static int tempID = 0;
	GameObjectLoadInfo info;
	info.ID = tempID++;
	objectInfos.push_back({ info });

	return objectInfos;
}

