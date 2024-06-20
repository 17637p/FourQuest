#include "EditorHelper.h"

#include <fstream>
#include <sstream>
#include <string>

#include "../FQCommon/FQPath.h"

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
