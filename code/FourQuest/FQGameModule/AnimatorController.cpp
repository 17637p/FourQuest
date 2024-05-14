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

	assert(iter != mParmeters.end() && "ParameterID ����");
	assert(iter->second.type() == parameter.type() && "Type�� ��ġ���� �ʽ��ϴ�");
	iter->second = parameter;
}

void fq::game_module::AnimatorController::AddParameter(ParameterID id, ParameterID parameter)
{
	assert(mParmeters.find(id) == mParmeters.end());
	mParmeters.insert({ id,parameter });
}
