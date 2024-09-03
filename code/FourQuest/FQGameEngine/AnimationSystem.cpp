#include "AnimationSystem.h"

#include "../FQGameModule/Animator.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/MaterialAnimator.h"

#include "GameProcess.h"
#include "RenderingSystem.h"
#include "ResourceSystem.h"

fq::game_engine::AnimationSystem::AnimationSystem()
	:mGameProcess(nullptr)
	, mScene(nullptr)
	, mGraphics(nullptr)
	, mChangeAnimationStateHandler{}
	, mStateQueue{}
{}

fq::game_engine::AnimationSystem::~AnimationSystem()
{}

void fq::game_engine::AnimationSystem::Initialize(GameProcess* game)
{
	mGameProcess = game;
	mScene = mGameProcess->mSceneManager->GetCurrentScene();
	mGraphics = mGameProcess->mGraphics;
}

void fq::game_engine::AnimationSystem::UpdateAnimation(float dt)
{
	// 1. State Update
	updateAnimtorState(dt);

	// 2. Animation을 적용
	processAnimation(dt);
}

void fq::game_engine::AnimationSystem::updateAnimtorState(float dt)
{
	using namespace fq::game_module;

	mScene->ViewComponents<Animator>(
		[dt](GameObject& object, Animator& animator)
		{
			animator.UpdateState(dt);
		}
	);
}

void fq::game_engine::AnimationSystem::processAnimation(float dt)
{
	using namespace fq::game_module;

	mScene->ViewComponents<Animator>(
		[dt](GameObject& object, Animator& animator)
		{
			if (animator.IsStopAnimation())
			{
				return;
			}

			animator.UpdateAnimation(dt);

			if (!animator.GetHasController())
			{
				spdlog::warn("{} does not have a controller", object.GetName());
				return;
			}
			else if (!animator.GetHasNodeHierarchyInstance())
			{
				spdlog::warn("animator does not have a node hierarchy instance ", object.GetName());
				return;
			}

			auto& controller = animator.GetController();
			auto& nodeHierarchyInstance = animator.GetNodeHierarchyInstance();
			float timePos = controller.GetTimePos();

			if (controller.IsInTransition())
			{
				float blendPos = controller.GetBlendTimePos();
				float blendWeight = controller.GetBlendWeight();
				nodeHierarchyInstance.Update(timePos, controller.GetSharedRefCurrentStateAnimation(), blendPos, controller.GetSharedRefNextStateAnimation(), blendWeight);
			}
			else if (controller.GetHasCurrentStateAnimation())
			{
				nodeHierarchyInstance.Update(timePos, controller.GetSharedRefCurrentStateAnimation());
			}
		});

	mScene->ViewComponents<UVAnimator>(
		[dt](GameObject& object, UVAnimator& animator)
		{
			animator.UpdateTimePos(dt * animator.GetPlaySpeed());
		});

	mScene->ViewComponents<MaterialAnimator>(
		[dt](GameObject& object, MaterialAnimator& animator)
		{
			animator.UpdateTimePos(dt);
		});
}

bool fq::game_engine::AnimationSystem::LoadAnimatorController(fq::game_module::GameObject* object)
{
	auto animator = object->GetComponent<fq::game_module::Animator>();
	auto controllerPath = animator->GetControllerPath();
	auto nodeHierarchyPath = animator->GetNodeHierarchyPath();

	if (!std::filesystem::exists(controllerPath))
	{
		spdlog::warn("{} animation controller load fail", object->GetName());
		return false;
	}
	if (!std::filesystem::exists(nodeHierarchyPath))
	{
		spdlog::warn("{} nodeHierarchy load fail", object->GetName());
		return false;
	}

	// 컨트롤러 로드
	auto controller = mLoader.Load(controllerPath);

	// 계층 구조 로드
	auto nodeHierarchyOrNull = mGameProcess->mResourceSystem->GetNodeHierarchy(nodeHierarchyPath);

	if (nodeHierarchyOrNull == nullptr)
	{
		mGameProcess->mResourceSystem->LoadNodeHierarchy(nodeHierarchyPath);
		nodeHierarchyOrNull = mGameProcess->mResourceSystem->GetNodeHierarchy(nodeHierarchyPath);
	}
	assert(nodeHierarchyOrNull != nullptr);

	auto nodeHierarchyInstance = nodeHierarchyOrNull->CreateNodeHierarchyInstance();
	animator->SetNodeHierarchy(nodeHierarchyOrNull);
	animator->SetNodeHierarchyInstance(nodeHierarchyInstance);
	nodeHierarchyInstance->SetBindPose();

	// 애니메이션 리소스 로딩 및 계층 구조 캐시 생성
	for (auto& [stateName, animationStateNode] : controller->GetStateMap())
	{
		const auto& animationPath = animationStateNode.GetAnimationPath();

		if (!std::filesystem::exists(animationPath))
		{
			continue;
		}

		auto animationInterfaceOrNull = mGameProcess->mResourceSystem->GetAnimation(animationPath);

		if (animationInterfaceOrNull == nullptr)
		{
			mGameProcess->mResourceSystem->LoadAnimation(animationPath);
			animationInterfaceOrNull = mGameProcess->mResourceSystem->GetAnimation(animationPath);
		}
		assert(animationInterfaceOrNull != nullptr);

		animationStateNode.SetAnimation(animationInterfaceOrNull);
		nodeHierarchyOrNull->RegisterAnimation(animationInterfaceOrNull);
	}

	controller->SetAnimator(animator);
	animator->SetController(controller);

	return true;
}