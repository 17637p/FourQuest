#include "AnimatorController.h"

fq::game_module::AnimatorController::AnimatorController()
{

}

fq::game_module::AnimatorController::~AnimatorController()
{

}

void fq::game_module::AnimatorController::SetParameter(ParameterID id, Parameter parameter)
{
	auto iter = mParmeters.find(id);

	assert(iter != mParmeters.end() && "ParameterID ¿À·ù");
	assert(iter->second.type() == parameter.type());
	iter->second = parameter;
}
