#include "AnimationSystem.h"

#include "GameProcess.h"
#include "RenderingSystem.h"

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

	// 2. Animation�� ����
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

			if (!animator.GetHasController() )
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
			else
			{
				if (controller.GetHasCurrentStateAnimation())
				{
					nodeHierarchyInstance.Update(timePos, controller.GetSharedRefCurrentStateAnimation());
				}
				else
				{
					int a = 0;
				}
			}
		});
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

	unsigned int modelKey = mGameProcess->mRenderingSystem->GetModelKey(animator->GetNodeHierarchyModelPath(), {});

	// �ִϸ��̼� ���ҽ� �ε�
	if (!mGameProcess->mGraphics->TryCreateModelResource( modelKey,animator->GetNodeHierarchyModelPath())) // to do : �� �κ� renderSystem�� Load �Լ� Ȱ���ϰ� ����
	{
		return false;
	}

	// ���������� �ν��Ͻ� ���� �� ���ε�
	auto nodeHierarchy = mGameProcess->mGraphics->GetNodeHierarchyByModelPathOrNull(modelKey);

	auto nodeHierarchyInstance = nodeHierarchy->CreateNodeHierarchyInstance();
	animator->SetNodeHierarchy(nodeHierarchy);
	animator->SetNodeHierarchyInstance(nodeHierarchyInstance);
	nodeHierarchyInstance->SetBindPose();

	auto controller = mLoader.Load(controllerPath);

	// �ִϸ��̼� ��忡 �ִϸ��̼� ���ҽ� ���ε�
	for (auto& [stateName, animationStateNode] : controller->GetStateMap())
	{
		const auto& modelPath = animationStateNode.GetModelPath();
		const auto& animName = animationStateNode.GetAnimationName();
		unsigned int key = mGameProcess->mRenderingSystem->GetModelKey(modelPath, {});

		if (!modelPath.empty() && !animName.empty())
		{
			mGameProcess->mGraphics->CreateModelResource(key,animationStateNode.GetModelPath()); // to do : �� �κ� renderSystem�� Load �Լ� Ȱ���ϰ� ����
			auto animationInterface = mGameProcess->mGraphics->GetAnimationByModelPathOrNull(key, animationStateNode.GetAnimationName());

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