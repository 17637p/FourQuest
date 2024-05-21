#include "AnimationTransition.h"

#include "AnimatorController.h"

bool fq::game_module::TransitionCondition::CheckCondition(Parameter parameter)const
{
	auto parameterType = parameter.type();
	auto compareType = GetCompareParameter().type();

	assert(parameterType == compareType);

	if (compareType == entt::resolve<int>())
	{
		int val = parameter.cast<int>();
		int compareVal = GetCompareParameter().cast<int>();

		switch (GetCheckType())
		{
		case fq::game_module::TransitionCondition::CheckType::Greater:
			if (val > compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Less:
			if (val < compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Equals:
			if (val == compareVal) return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::NotEqual:
			if (val != compareVal) return true;
			break;
		}
	}
	else if (compareType == entt::resolve<float>())
	{
		float val = parameter.cast<float>();
		float compareVal = GetCompareParameter().cast<float>();

		switch (GetCheckType())
		{
		case fq::game_module::TransitionCondition::CheckType::Greater:
			if (val > compareVal)return true;
			break;
		case fq::game_module::TransitionCondition::CheckType::Less:
			if (val < compareVal) return true;
			break;
		}
	}
	else if (compareType == entt::resolve<bool>())
	{
		bool val = parameter.cast<bool>();
		bool compareVal = GetCompareParameter().cast<bool>();

		switch (GetCheckType())
		{
			case fq::game_module::TransitionCondition::CheckType::Equals:
				if (val == compareVal) return true;
				break;
			case fq::game_module::TransitionCondition::CheckType::NotEqual:
				if (val != compareVal) return true;
				break;
		}
	}
	else if (compareType == entt::resolve<char>())
	{
		char val = parameter.cast<char>();
		char compareVal = GetCompareParameter().cast<char>();

		if (val == compareVal) return true;
	}
	else assert(nullptr);

	return false;
}

fq::game_module::TransitionCondition::TransitionCondition(CheckType type, ParameterID compareID, Parameter comparCondition)
	:mCheckType(type)
	, mCompareParameter(comparCondition)
	, mParameterID(compareID)
{}

void fq::game_module::TransitionCondition::SetCompareParameter(Parameter val)
{
	if (val.type() == entt::resolve<int>())
	{
		mCheckType = CheckType::Equals;
	}
	else if (val.type() == entt::resolve<float>())
	{
		mCheckType = CheckType::Greater;
	}
	else if (val.type() == entt::resolve<bool>())
	{
		mCheckType = CheckType::Equals;
	}
	else if (val.type() == entt::resolve<char>())
	{
		mCheckType = CheckType::Equals;
	}
	else assert(nullptr);

	mCompareParameter = val;
}

void fq::game_module::AnimationTransition::PushBackCondition(TransitionCondition::CheckType type
	, TransitionCondition::ParameterID id
	, TransitionCondition::Parameter compareParam)
{
	mConditions.push_back({ type, id, compareParam });
}

void fq::game_module::AnimationTransition::PopBackCondition()
{
	if (mConditions.empty()) return;

	mConditions.pop_back();
}

fq::game_module::AnimationTransition::AnimationTransition(std::string prev, std::string next)
	:mEnterState(next)
	,mExitState(prev)
	,mConditions{}
{}

fq::game_module::AnimationTransition::AnimationTransition()
	:mEnterState{}
	,mExitState{}
	,mConditions{}
{
}
