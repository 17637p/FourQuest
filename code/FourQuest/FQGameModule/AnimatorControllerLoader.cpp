#include "AnimatorControllerLoader.h"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <assert.h>

#include "../FQCommon/FQPath.h"
#include "AnimatorController.h"
#include "AnimationTransition.h"
#include "AnimationStateNode.h"
#include "IStateBehaviour.h"

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

		std::string path;
		if (value.find("animationPath") != value.end())
		{
			path = value.at("animationPath");
			path = fq::path::GetAbsolutePath(path).string();
		}

		float playbackSpeed = value.at("playbackSpeed").get<float>();
		float duration = value.at("duration").get<float>();
		float startTimepos = 0.f;
		if (value.find("startTimePos") != value.end())
		{
			startTimepos = value.at("startTimePos").get<float>();
		}
		bool isLoof = value.at("isLoof").get<bool>();

		stateNode.SetType(AnimationStateNode::Type::State);
		stateNode.SetAnimationKey(stateName);
		stateNode.SetPlayBackSpeed(playbackSpeed);
		stateNode.SetAnimationPath(path);
		stateNode.SetStartTimePos(startTimepos);
		stateNode.SetDuration(duration);
		stateNode.SetLoof(isLoof);

		// StateBehaviours
		auto& stateMap = stateNode.GetStateBehaviourMap();

		if (value.find("stateBehaviours") != value.end())
		{
			for (const auto& element : value.at("stateBehaviours").items())
			{
				const std::string& stateID = element.key();
				entt::id_type id = std::stoul(stateID);

				if (!entt::resolve(id))
					continue;

				entt::meta_any anyState = mConverter.ParseClassFromJson(stateID, element.value());

				entt::meta_type type = anyState.type();
				entt::meta_type iStateType = entt::resolve<IStateBehaviour>();
				assert(type.can_cast(iStateType));

				IStateBehaviour* state = anyState.try_cast<IStateBehaviour>();
				assert(state);

				auto clone = state->Clone();

				stateMap.insert({ id,clone });
			}
		}

		controller->AddStateNode(stateNode);
	}

	// 3. Transition 로드 
	auto transitionsJson = controllerJson.find("transitions");
	for (const auto& value : transitionsJson.value())
	{
		std::string exit = value.at("exitState");
		std::string enter = value.at("enterState");
		float exitTime = value.at("exitTime");
		float transitionDuration = value.at("transitionDuration");
		int interruptionSource = value.at("InterruptionSource");
		bool canTransitionToSelf = true;
		if (value.find("canTransitionToSelf") != value.end())
		{
			canTransitionToSelf = value.at("canTransitionToSelf");
		}

		AnimationTransition transition{};

		transition.SetExitState(exit);
		transition.SetEnterState(enter);
		transition.SetExitTime(exitTime);
		transition.SetTransitionDuration(transitionDuration);
		transition.SetInterruptionSource(static_cast<AnimationTransition::InterruptionSource>(interruptionSource));
		transition.SetCanTrasitionToSelf(canTransitionToSelf);

		json conditionsJson = value.at("conditions");
		for (auto& conditionJson : conditionsJson)
		{
			auto checkType = static_cast<TransitionCondition::CheckType>(conditionJson.at("type").get<int>());
			std::string id = conditionJson.at("parameterID");
			json parameterJson = conditionJson.at("parameter");
			entt::meta_any parameter;

			if (parameterJson.is_number_float())
			{
				parameter = parameterJson.get<float>();
			}
			else if (parameterJson.is_number_integer())
			{
				parameter = parameterJson.get<int>();
			}
			else if (parameterJson.is_boolean())
			{
				parameter = parameterJson.get<bool>();
			}
			else
			{
				parameter = AnimatorController::OnTrigger;
			}
			transition.PushBackCondition(checkType, id, parameter);
		}

		controller->AddTransition(transition);
	}

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
		stateJson["animationPath"] = fq::path::GetRelativePath(stateNode.GetAnimationPath()).string();
		stateJson["playbackSpeed"] = stateNode.GetPlayBackSpeed();
		stateJson["startTimePos"] = stateNode.GetStartTimePos();
		stateJson["duration"] = stateNode.GetDuration();
		stateJson["isLoof"] = stateNode.IsLoof();

		// StateBehaviour
		stateJson["stateBehaviours"] = serializeStateBehaviours(stateNode);
		stateMapJson[stateName] = stateJson;
	}
	controllerJson["stateMap"] = stateMapJson;

	// 3. Transition
	ordered_json transitionsJson;
	const auto& transitions = controller.GetTransitionMap();

	for (const auto& [name, transition] : transitions)
	{
		ordered_json transitionJson;

		transitionJson["exitState"] = transition.GetExitState();
		transitionJson["enterState"] = transition.GetEnterState();
		transitionJson["exitTime"] = transition.GetExitTime();
		transitionJson["transitionDuration"] = transition.GetTransitionDuration();
		transitionJson["InterruptionSource"] = static_cast<int>(transition.GetInterruptionSource());
		transitionJson["canTransitionToSelf"] = transition.CanTrasitionToSelf();

		ordered_json conditionsJson;
		// 조건 저장 
		for (const auto& condition : transition.GetConditions())
		{
			ordered_json conditionJson;

			conditionJson["type"] = static_cast<int>(condition.GetCheckType());
			conditionJson["parameterID"] = condition.GetParameterID();
			auto compareParameter = condition.GetCompareParameter();

			if (compareParameter.type() == entt::resolve<int>())
				conditionJson["parameter"] = compareParameter.cast<int>();
			else if (compareParameter.type() == entt::resolve<float>())
				conditionJson["parameter"] = compareParameter.cast<float>();
			else if (compareParameter.type() == entt::resolve<bool>())
				conditionJson["parameter"] = compareParameter.cast<bool>();
			else
				conditionJson["parameter"] = "trigger";

			conditionsJson.push_back(conditionJson);
		}
		transitionJson["conditions"] = conditionsJson;

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

nlohmann::json fq::game_module::AnimatorControllerLoader::serializeStateBehaviours(const AnimationStateNode& node)
{
	json out;

	for (const auto& [id, stateBehaviour] : node.GetStateBehaviourMap())
	{
		entt::meta_type behaviourType = entt::resolve(id);
		assert(behaviourType);

		entt::meta_any anyBehaviour = behaviourType.from_void(stateBehaviour.get());
		assert(anyBehaviour);

		mConverter.ParseClassToJson(anyBehaviour, out);
	}

	return out;
}

