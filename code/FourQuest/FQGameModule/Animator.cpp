#include "Animator.h"

#include "../FQReflect/entt.hpp"
#include "AnimatorController.h"

fq::game_module::Animator::Animator()
	:mDefaultPlaySpeed(1.f)
	, mbIsStopAnimation(false)
	, mNodeHierarchyInstance{ nullptr }
	, mController{ nullptr }
	, mNodeHierarchy{ nullptr }
	, mbUpdateAnimationCPUData(true)
	, mbUpdateAnimationGPUData(false)
	, mbCreateAnimationTexture(false)
	, mLowerController{ nullptr }
	, mLowerControllerPath{}
	, mbUseLowerController(false)
{}

fq::game_module::Animator::~Animator()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::Animator::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAnimator = std::dynamic_pointer_cast<Animator>(clone);

	if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneAnimator = ObjectPool::GetInstance()->Assign<Animator>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneAnimator = *this;
	}

	cloneAnimator->mController = nullptr;
	cloneAnimator->mNodeHierarchyInstance = nullptr;

	return cloneAnimator;
}

void fq::game_module::Animator::SetController(std::shared_ptr<AnimatorController> controller)
{
	mController = controller;
}

void fq::game_module::Animator::SetParameterInt(const std::string& id, int val)
{
	bool upperResult = false;
	bool lowerResult = false;

	if (mController != nullptr)
	{
		upperResult = mController->SetParameter(id, val);
	}

	if (mLowerController != nullptr)
	{
		lowerResult = mLowerController->SetParameter(id, val);
	}

	if (!upperResult && !lowerResult)
	{
		spdlog::warn("{} Parameter Can't Finded", id);
	}
}

void fq::game_module::Animator::SetParameterFloat(const std::string& id, float val)
{
	bool upperResult = false;
	bool lowerResult = false;

	if (mController != nullptr)
	{
		upperResult = mController->SetParameter(id, val);
	}

	if (mLowerController != nullptr)
	{
		lowerResult = mLowerController->SetParameter(id, val);
	}

	if (!upperResult && !lowerResult)
	{
		spdlog::warn("{} Parameter Can't Finded", id.c_str());
	}
}

void fq::game_module::Animator::SetParameterBoolean(const std::string& id, bool val)
{
	bool upperResult = false;
	bool lowerResult = false;

	if (mController != nullptr)
	{
		upperResult = mController->SetParameter(id, val);
	}

	if (mLowerController != nullptr)
	{
		lowerResult = mLowerController->SetParameter(id, val);
	}

	if (!upperResult && !lowerResult)
	{
		spdlog::warn("{} Parameter Can't Finded", id);
	}
}

void fq::game_module::Animator::SetParameterTrigger(const std::string& id)
{
	bool upperResult = false;
	bool lowerResult = false;

	if (mController != nullptr)
	{
		upperResult = mController->SetParameter(id, AnimatorController::OnTrigger);
	}

	if (mLowerController != nullptr)
	{
		lowerResult = mLowerController->SetParameter(id, AnimatorController::OnTrigger);
	}

	if (!upperResult && !lowerResult)
	{
		spdlog::warn("{} Parameter Can't Finded", id);
	}
}

void fq::game_module::Animator::SetParameterOffTrigger(const std::string& id)
{
	bool upperResult = false;
	bool lowerResult = false;

	if (mController != nullptr)
	{
		upperResult = mController->SetParameter(id, AnimatorController::OffTrigger);
	}

	if (mLowerController != nullptr)
	{
		lowerResult = mLowerController->SetParameter(id, AnimatorController::OffTrigger);
	}

	if (!upperResult && !lowerResult)
	{
		spdlog::warn("{} Parameter Can't Finded", id);
	}
}

void fq::game_module::Animator::UpdateState(float dt)
{
	if (mController != nullptr)
	{
		mController->UpdateState(dt);
	}

	if (mbUseLowerController && mLowerController)
	{
		mLowerController->UpdateState(dt);
	}
}

void fq::game_module::Animator::UpdateAnimation(float dt)
{
	if (mController != nullptr)
		mController->UpdateAnimation(dt, mDefaultPlaySpeed);

	if (mbUseLowerController && mLowerController)
	{
		mLowerController->UpdateAnimation(dt, mDefaultPlaySpeed);
	}
}

bool fq::game_module::Animator::IsInTransition() const
{
	return mController->IsInTransition();
}

bool fq::game_module::Animator::IsLowerInTransition() const
{
	return mLowerController->IsInTransition();
}

void fq::game_module::Animator::OnUpdate(float dt)
{
	if (mController != nullptr)
	{
		mController->Update(dt);
	}

	if (mbUseLowerController && mLowerController)
	{
		mLowerController->Update(dt);
	}
}

void fq::game_module::Animator::SetNodeHierarchyPath(const std::string& path)
{
	if (std::filesystem::path(path).extension() == ".nodeHierachy")
		mNodeHierarchyPath = path;
}

void fq::game_module::Animator::ProcessAnimationEvent(class GameObject* gameObject, fq::game_module::EventManager* eventManager)
{
	if (mController != nullptr)
	{
		mController->ProcessAnimationEvent(gameObject, eventManager);
	}

	if (mLowerController && mbUseLowerController)
	{
		mLowerController->ProcessAnimationEvent(gameObject, eventManager);
	}
}

void fq::game_module::Animator::OnDestroy()
{
	if (mController)
	{
		auto& stateMap = mController->GetStateMap();
		auto name = mController->GetCurrentStateName();

		if (auto iter = stateMap.find(name); iter != stateMap.end())
		{
			iter->second.OnStateExit();
		}
	}
	if (mbUseLowerController && mLowerController)
	{
		auto& stateMap = mLowerController->GetStateMap();
		auto name = mLowerController->GetCurrentStateName();

		if (auto iter = stateMap.find(name); iter != stateMap.end())
		{
			iter->second.OnStateExit();
		}
	}
}

void fq::game_module::Animator::SetUseLowerController(bool bUseLower)
{
	mbUseLowerController = bUseLower;
}

fq::game_module::AnimatorController& fq::game_module::Animator::GetLowerController() const
{
	return *mLowerController;
}

void fq::game_module::Animator::SetLowerController(std::shared_ptr<AnimatorController> lowerController)
{
	mLowerController = lowerController;
}

