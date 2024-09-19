#include "EditorHelper.h"

#include <fstream>
#include <sstream>
#include <string>

#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQPath.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Socket.h"
#include "GameProcess.h"
#include "EditorProcess.h"

std::string fq::game_engine::EditorHelper::GetStartSceneName()
{
	auto internalPath = fq::path::GetInternalPath() / "init" / "scene.txt";

	std::string startScene;

	if (std::filesystem::exists(internalPath))
	{
		std::ifstream inFile(internalPath);

		std::stringstream buffer;
		buffer << inFile.rdbuf();
		startScene = buffer.str();
	}
	else
	{
		auto scenePath = fq::path::GetInternalPath() / "scene";

		startScene = fq::path::GetFileList(scenePath).begin()->filename().string();
	}

	return startScene;
}

void fq::game_engine::EditorHelper::SetStartSceneName(std::string name)
{
	auto internalPath = fq::path::GetInternalPath() / "init" / "scene.txt";
	std::ofstream outFile(internalPath);
	outFile << name;
	outFile.close();
}

void fq::game_engine::EditorHelper::UpdateEditorMode(GameProcess* game, float dt)
{
	auto scene = game->mSceneManager->GetCurrentScene();

	/// Editor의 동기화를 위한 처리를 합니다

	for (auto& object : scene->GetComponentView<game_module::Socket>())
	{
		auto socket = object.GetComponent<game_module::Socket>();
		socket->BindBone();
		socket->OnUpdate(dt);
	}
}


using json = nlohmann::json;
namespace fs = std::filesystem;

void fq::game_engine::EditorHelper::LoadEditorSetting(EditorProcess* editor)
{
	// Editor 세팅 정보 불러오기 

	auto path = fq::path::GetInternalPath() / "init" / "editor_setting.txt";

	std::ifstream input(path);
	json setting;

	if (input.is_open())
	{
		input >> setting;
		input.close();
	}
	else
	{
		editor->mModelSystem->ConvertAllModel();
		return;
	}

	// Version 확인
	if (setting["version"] != EditorVersion)
	{
		editor->mModelSystem->ConvertAllModel();
	}

	// Debug 정보 
	auto& debugSystem = editor->mDebugSystem;

	if (setting.find("OnBoxCollider") != setting.end())
		debugSystem->GetOnBoxCollider() = setting["OnBoxCollider"];

	if (setting.find("OnCapsuleCollider") != setting.end())
		debugSystem->GetOnCapsuleCollider() = setting["OnCapsuleCollider"];

	if (setting.find("OnCharaterController") != setting.end())
		debugSystem->GetOnCharaterController() = setting["OnCharaterController"];

	if (setting.find("OnConvexMeshCollider") != setting.end())
		debugSystem->GetOnConvexMeshCollider() = setting["OnConvexMeshCollider"];

	if (setting.find("OnGrid") != setting.end())
		debugSystem->GetOnGrid() = setting["OnGrid"];

	if (setting.find("OnSphereCollider") != setting.end())
		debugSystem->GetOnSphereCollider() = setting["OnSphereCollider"];

	if (setting.find("OnSphereCollider") != setting.end())
		debugSystem->GetOnSphereCollider() = setting["OnSphereCollider"];

	if (setting.find("OnTerrainCollider") != setting.end())
		debugSystem->GetOnTerrainCollider() = setting["OnTerrainCollider"];

	if (setting.find("OnNavigationMesh") != setting.end())
		debugSystem->GetOnNavigationMesh() = setting["OnNavigationMesh"];

}

void fq::game_engine::EditorHelper::SaveEditorSetting(EditorProcess* editor)
{
	// Editor 세팅 정보 저장 

	json out;

	out["version"] = EditorVersion;

	// Debug 정보
	auto& debugSystem = editor->mDebugSystem;

	out["OnBoxCollider"] = debugSystem->GetOnBoxCollider();
	out["OnCapsuleCollider"] = debugSystem->GetOnCapsuleCollider();
	out["OnCharaterController"] = debugSystem->GetOnCharaterController();
	out["OnConvexMeshCollider"] = debugSystem->GetOnConvexMeshCollider();
	out["OnGrid"] = debugSystem->GetOnGrid();
	out["OnSphereCollider"] = debugSystem->GetOnSphereCollider();
	out["OnTerrainCollider"] = debugSystem->GetOnTerrainCollider();
	out["OnNavigationMesh"] = debugSystem->GetOnNavigationMesh();


	auto path = fq::path::GetInternalPath() / "init" / "editor_setting.txt";
	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << out;
		output.close();
	}
}
