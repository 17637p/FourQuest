#include "AnimatorControllerLoader.h"

#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <assert.h>

#include "AnimatorController.h"
#include "AnimationTransition.h"
#include "AnimationStateNode.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

std::shared_ptr<fq::game_module::AnimatorController> fq::game_module::AnimatorControllerLoader::Load(const Path& path)
{
	assert(fs::exists(path));
	assert(path.extension() == ".controller");

	json j;


	// 1. 파라미터 로드

	// 2. State 로드

	// 3. Transition 로드 

	return nullptr;
}

void fq::game_module::AnimatorControllerLoader::Save(const AnimatorController& controller, const Path& path)
{
	json j;

	// 1. 파라미터 
	auto parameterPack = controller.GetParameterPack();

	for (auto& [id, parameter] : parameterPack)
	{
		if (parameter.type() == entt::resolve<int>())
		{
			j[id] = parameter.cast<int>();
		}
		else if (parameter.type() == entt::resolve<float>())
		{
			j[id] = parameter.cast<float>();
		}
		else if (parameter.type() == entt::resolve<bool>())
		{
			j[id] = parameter.cast<bool>();
		}
		else 
		{
			j[id] = parameter.cast<char>();
		}
	}



	// 2. State 

	// 3. Transition


	std::ofstream output(path);

	if (output.is_open())
	{
		output << std::setw(4) << j;
		output.close();
	}
	else
		assert(nullptr);

}

