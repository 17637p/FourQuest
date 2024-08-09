#include "SceneResourceList.h"

#include <fstream>
#include <iostream>
#include "../FQCommon/FQPath.h"
#include "../FQReflect/FQReflect.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void fq::game_engine::SceneResourceList::Load(const Path& path)
{
	json j;

	std::ifstream readFile(path);
	json  readJson;
	if (readFile.is_open())
	{
		readFile >> readJson;
		readFile.close();
	}

	if (auto json = readJson.find("Model"); json != readJson.end())
	{
		for (const auto& path : json.value())
		{
			auto absolutePath = fq::path::GetAbsolutePath(path);
			modelPaths.insert(absolutePath.string());
		}
	}

	if (auto json = readJson.find("Material"); json != readJson.end())
	{
		for (const auto& path : json.value())
		{
			auto absolutePath = fq::path::GetAbsolutePath(path);
			materialPaths.insert(absolutePath.string());
		}
	}

	if (auto json = readJson.find("AnimatorController"); json != readJson.end())
	{
		for (const auto& path : json.value())
		{
			auto absolutePath = fq::path::GetAbsolutePath(path);
			animatorControllerPaths.insert(absolutePath.string());
		}
	}

	if (auto json = readJson.find("Animation"); json != readJson.end())
	{
		for (const auto& path : json.value())
		{
			auto absolutePath = fq::path::GetAbsolutePath(path);
			animationPaths.insert(absolutePath.string());
		}
	}

	if (auto json = readJson.find("NodeHierachy"); json != readJson.end())
	{
		for (const auto& path : json.value())
		{
			auto absolutePath = fq::path::GetAbsolutePath(path);
			nodeHierachyPaths.insert(absolutePath.string());
		}
	}
}

void fq::game_engine::SceneResourceList::Save(const Path& path)
{
	json saveJson;

	json model;
	for (const auto& path : modelPaths)
	{
		auto relative = fq::path::GetRelativePath(path);
		model.push_back(relative);
	}
	saveJson["Model"] = model;

	json material;
	for (const auto& path : materialPaths)
	{
		auto relative = fq::path::GetRelativePath(path);
		material.push_back(relative);
	}
	saveJson["Material"] = material;

	json animatorController;
	for (const auto& path : animatorControllerPaths)
	{
		auto relative = fq::path::GetRelativePath(path);
		animatorController.push_back(relative);
	}
	saveJson["AnimatorController"] = animatorController;

	json animation;
	for (const auto& path : animationPaths)
	{
		auto relative = fq::path::GetRelativePath(path);
		animation.push_back(relative);
	}
	saveJson["Animation"] = animation;

	json nodeHiera;
	for (const auto& path : animationPaths)
	{
		auto relative = fq::path::GetRelativePath(path);
		animation.push_back(relative);
	}
	saveJson["Animation"] = animation;

	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << saveJson;
		output.close();
	}
}
