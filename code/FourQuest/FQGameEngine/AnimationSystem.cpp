#include "AnimationSystem.h"

#include "GameProcess.h"

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

	mChangeAnimationStateHandler = mGameProcess->mEventManager
		->RegisterHandle<fq::event::ChangeAnimationState>(this, &AnimationSystem::ChangeAnimationState);
}

void fq::game_engine::AnimationSystem::UpdateAnimation(float dt)
{
	// 1. State Update
	updateAnimtorState(dt);

	// 2. Animation 변경요청  
	processCallBack();

	// 3. Animation을 적용
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
			animator.UpdateAnimation(dt);

			if (!animator.GetHasController())
			{
				// log
				return;
			}

			auto& controller = animator.GetController();

			auto nodeHierarchyInstance = animator.GetNodeHierarchyInstance();
			float timePos = controller.GetTimePos();

			if (controller.IsInTransition())
			{
				float blendPos = controller.GetBlendTimePos();
				float blendWeight = controller.GetBlendWeight();
				animator.GetNodeHierarchyInstance()->Update(timePos, controller.GetCurrentStateAnimation(), blendPos, controller.GetNextStateAnimationOrNull(), blendWeight);
			}
			else
			{
				if (controller.GetCurrentStateAnimation() != nullptr)
				{
					animator.GetNodeHierarchyInstance()->Update(timePos, controller.GetCurrentStateAnimation());
				}
			}
		});
}


void fq::game_engine::AnimationSystem::ChangeAnimationState(const fq::event::ChangeAnimationState& event)
{
	mStateQueue.push(event);
}

bool fq::game_engine::AnimationSystem::LoadAnimatorController(fq::game_module::GameObject* object)
{
	auto animator = object->GetComponent<fq::game_module::Animator>();

	auto controllerPath = animator->GetControllerPath();

	if (!std::filesystem::exists(controllerPath))
	{
		spdlog::warn("{} animation controller load fail", object->GetName());
		return false;
	}

	if (!mGameProcess->mGraphics->TryCreateModelResource(animator->GetModelPath()))
	{
		return false;
	}

	auto nodeHierarchy = mGameProcess->mGraphics->GetNodeHierarchyByModelPathOrNull(animator->GetModelPath());
	auto nodeHierarchyInstance = nodeHierarchy->CreateNodeHierarchyInstance();
	animator->SetNodeHierarchy(nodeHierarchy);
	animator->SetNodeHierarchyInstance(nodeHierarchyInstance);

	auto controller = mLoader.Load(controllerPath);

	for (auto& [stateName, animationStateNode] : controller->GetStateMap())
	{
		const auto& modelPath = animationStateNode.GetModelPath();
		const auto& animName = animationStateNode.GetAnimationName();

		if (!modelPath.empty() && !animName.empty())
		{
			mGameProcess->mGraphics->CreateModelResource(animationStateNode.GetModelPath());
			auto animationInterface = mGameProcess->mGraphics->GetAnimationByModelPathOrNull(animationStateNode.GetModelPath(), animationStateNode.GetAnimationName());

			if (animationInterface != nullptr)
			{
				animationStateNode.SetAnimation(animationInterface);
				nodeHierarchy->RegisterAnimation(animationInterface);
			}
		}
	}

	controller->SetAnimator(animator);
	animator->SetController(controller);

	return true;
}

void fq::game_engine::AnimationSystem::processCallBack()
{
	return;
	// while (!mStateQueue.empty())
	// {
	// 	auto event = mStateQueue.front();
	// 	mStateQueue.pop();
	// 
	// 	auto animator = event.animator;
	// 
	// 	if (animator->GetGameObject()->IsDestroyed())
	// 	{
	// 		continue;
	// 	}
	// 	animator->gets
	// 
	// 		const auto& meshs = animator->GetSkinnedMeshs();
	// 	for (auto& mesh : meshs)
	// 	{
	// 		// IAnimation ref 
	// 		if (event.bIsBlend)
	// 		{
	// 			mesh->GetSkinnedMeshObject()->SetBlendAnimationKey(event.currentState, event.nextState);
	// 		}
	// 		else
	// 		{
	// 			mesh->GetSkinnedMeshObject()->SetAnimationKey(event.currentState);
	// 		}
	// 	}
	// }
}
