#include "AnimatorControllerLoader.h"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <assert.h>

#include "AnimatorController.h"
#include "AnimationTransition.h"
#include "AnimationStateNode.h"

using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;
namespace fs = std::filesystem;

std::shared_ptr<fq::game_module::AnimatorController> fq::game_module::AnimatorControllerLoader::Load(const Path& path)
{
	assert(fs::exists(path));
	assert(path.extension() == ".controller");

	std::ifstream readData(path);

	ordered_json controllerJson;

	if (readData.is_open())
	{
		readData >> controllerJson;
		readData.close();
	}
	else
		assert(!"파일 열기 실패");

	std::shared_ptr<AnimatorController> controller = std::make_shared<AnimatorController>();

	// 1. 파라미터 로드
	auto parameterJson = controllerJson.find("parameter");

	for (const auto& [key, value] : parameterJson.value().items())
	{
		std::string parameterName = key;

		entt::meta_any parameter;

		if (value.is_number_float())
		{
			parameter = value.get<float>();
		}
		else if (value.is_number_integer())
		{
			parameter = value.get<int>();
		}
		else if (value.is_string())
		{
			// Trigger
			parameter = AnimatorController::OffTrigger;
		}
		else if (value.is_boolean())
		{
			parameter = value.get<bool>();
		}
		else assert(nullptr);

		controller->AddParameter(parameterName, parameter);
	}

	// 2. State 로드
	auto stateMapJson = controllerJson.find("stateMap");
	for (const auto& [key, value] : stateMapJson.value().items())
	{
		AnimationStateNode stateNode(controller.get());
		std::string stateName = key;

	 	std::string path = value.at("modelPath");
		std::string name = value.at("animationName");
		float playbackSpeed = value.at("playbackSpeed").get<float>();

		stateNode.SetType(AnimationStateNode::Type::State);
		stateNode.SetAnimationKey(stateName);
		stateNode.SetPlayBackSpeed(playbackSpeed);
		stateNode.SetModelPath(path);
		stateNode.SetAnimationName(name);

		controller->AddStateNode(stateNode);
	}

	// 3. Transition 로드 
	auto transitionsJson = controllerJson.find("transitions");


	return controller;
}

void fq::game_module::AnimatorControllerLoader::Save(const AnimatorController& controller, const Path& path)
{
	ordered_json controllerJson;

	// 1. 파라미터 
	ordered_json parameterJson;
	auto parameterPack = controller.GetParameterPack();

	for (auto& [id, parameter] : parameterPack)
	{
		if (parameter.type() == entt::resolve<int>())
		{
			parameterJson[id] = parameter.cast<int>();
		}
		else if (parameter.type() == entt::resolve<float>())
		{
			parameterJson[id] = parameter.cast<float>();
		}
		else if (parameter.type() == entt::resolve<bool>())
		{
			parameterJson[id] = parameter.cast<bool>();
		}
		else if (parameter.type() == entt::resolve<char>())
		{
			std::string trigger;
			trigger.push_back(parameter.cast<char>());
			parameterJson[id] = trigger;
		}
	}
	controllerJson["parameter"] = parameterJson;

	// 2. StateMap
	ordered_json stateMapJson;
	const auto& stateMap = controller.GetStateMap();

	for (auto& [stateName, stateNode] : stateMap)
	{
		if (stateNode.GetType() != AnimationStateNode::Type::State) continue;

		ordered_json stateJson;
		stateJson["modelPath"] = stateNode.GetModelPath();
		stateJson["animationName"] = stateNode.GetAnimationName();
		stateJson["playbackSpeed"] = stateNode.GetPlayBackSpeed();
		stateMapJson[stateName] = stateJson;
	}
	controllerJson["stateMap"] = stateMapJson;

	// 3. Transition
	ordered_json transitionsJson;
	const auto& transitions = controller.GetTransitions();

	for (const auto& transition : transitions)
	{
		ordered_json transitionJson;

		transitionJson["exitState"] = transition.GetExitState();
		transitionJson["enterState"] = transition.GetEnterState();

		// 조건 저장 
		for (const auto& condition : transition.GetConditions())
		{
			ordered_json conditionJson;

			conditionJson["type"] = static_cast<int>(condition.GetCheckType());
			conditionJson["parameterID"] = condition.GetParameterID();
			auto compareParameter = condition.GetCompareParameter();

			if (compareParameter.type() == entt::resolve<int>())
				conditionJson["int"] = compareParameter.cast<int>();
			else if (compareParameter.type() == entt::resolve<float>())
				conditionJson["float"] = compareParameter.cast<float>();
			else if (compareParameter.type() == entt::resolve<bool>())
				conditionJson["bool"] = compareParameter.cast<bool>();
			else
				conditionJson["char"] = compareParameter.cast<char>();

			transitionJson["condition"] = conditionJson;
		}

		transitionsJson.push_back(transitionJson);
	}
	controllerJson["transitions"] = transitionsJson;

	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << controllerJson;
		output.close();
	}
	else
		assert(nullptr);
}

