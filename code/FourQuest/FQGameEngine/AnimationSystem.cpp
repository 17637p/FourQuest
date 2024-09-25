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

	fq::game_module::EventManager* eventManagerPtr = mGameProcess->mEventManager.get();

	mScene->ViewComponents<Animator>(
		[dt, eventManagerPtr](GameObject& object, Animator& animator)
		{
			if (!object.IsDestroyed())
			{
				animator.UpdateState(dt);
				animator.ProcessAnimationEvent(&object, eventManagerPtr);
			}
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
			bool useLowerController = animator.UseLowerController();

			// 상하체 분리 X 
			if (!useLowerController)
			{
				float timePos = controller.GetTimePos();
				if (controller.IsInTransition())
				{
					float blendPos = controller.GetBlendTimePos();
					float blendWeight = controller.GetBlendWeight();

					if (animator.GetUpdateAnimationCPUData())
					{
						nodeHierarchyInstance.Update(timePos, controller.GetSharedRefCurrentStateAnimation(), blendPos, controller.GetSharedRefNextStateAnimation(), blendWeight);
					}
					if (animator.GetUpdateAnimationGPUData())
					{
						nodeHierarchyInstance.UpdateGPUData(timePos, controller.GetSharedRefCurrentStateAnimation(), blendPos, controller.GetSharedRefNextStateAnimation(), blendWeight);
					}
				}
				else if (controller.GetHasCurrentStateAnimation())
				{
					if (animator.GetUpdateAnimationCPUData())
					{
						nodeHierarchyInstance.Update(timePos, controller.GetSharedRefCurrentStateAnimation());
					}
					if (animator.GetUpdateAnimationGPUData())
					{
						nodeHierarchyInstance.UpdateGPUData(timePos, controller.GetSharedRefCurrentStateAnimation());
					}
				}
			}
			else // 상하체 분리 O 
			{
				unsigned int upperStartIndex = nodeHierarchyInstance.GetNodeHierarchy()->GetSpineIndex();
				unsigned int upperEndIndex = nodeHierarchyInstance.GetNodeHierarchy()->GetUpperBodyEndIndex();
				unsigned int lowerStartIndex = nodeHierarchyInstance.GetNodeHierarchy()->GetLowerBodyStartIndex();
				unsigned int lowerEndIndex = nodeHierarchyInstance.GetNodeHierarchy()->GetEndIndex();

				// 1.애니메이션 행렬을 로컬 애니메이션을 초기상태로 만들어둡니다.
				nodeHierarchyInstance.SetBindPoseLocalTransform();

				// 2. 상체 애니메이션 
				float timePos = controller.GetTimePos();
				if (controller.IsInTransition())
				{
					float blendTimePos = controller.GetBlendTimePos();
					float blendWeight = controller.GetBlendWeight();
					nodeHierarchyInstance.UpdateLocalTransformRange(timePos
						, controller.GetSharedRefCurrentStateAnimation()
						, blendTimePos
						, controller.GetSharedRefNextStateAnimation()
						, blendWeight
						, upperStartIndex, upperEndIndex);
				}
				else if (controller.GetHasCurrentStateAnimation())
				{
					nodeHierarchyInstance.UpdateLocalTransformRange(timePos
						, controller.GetSharedCurrentStateAnimation()
						, upperStartIndex
						, upperEndIndex);
				}

				// 3. 하체 애니메이션 
				auto& lowerController = animator.GetLowerController();
				float lowerTimePos = lowerController.GetTimePos();

				if (lowerController.IsInTransition())
				{
					float blendTimePos = lowerController.GetBlendTimePos();
					float blendWeight = lowerController.GetBlendWeight();

					nodeHierarchyInstance.UpdateLocalTransformRange(lowerTimePos
						, lowerController.GetSharedRefCurrentStateAnimation()
						, blendTimePos
						, lowerController.GetSharedRefNextStateAnimation()
						, blendWeight
						, lowerStartIndex, lowerEndIndex);

					nodeHierarchyInstance.UpdateLocalTransformRange(lowerTimePos
						, lowerController.GetSharedRefCurrentStateAnimation()
						, blendTimePos
						, lowerController.GetSharedRefNextStateAnimation()
						, blendWeight
						, upperStartIndex, upperStartIndex);
				}
				else if (lowerController.GetHasCurrentStateAnimation())
				{
					nodeHierarchyInstance.UpdateLocalTransformRange(lowerTimePos
						, lowerController.GetSharedCurrentStateAnimation()
						, lowerStartIndex
						, lowerEndIndex);

					nodeHierarchyInstance.UpdateLocalTransformRange(lowerTimePos
						, lowerController.GetSharedCurrentStateAnimation()
						, upperStartIndex
						, upperStartIndex);
				}

				// 4. 갱신 해둔 로컬 애니메이션 행렬로 애니메이션에 사용할 toRoot 트랜스폼을 갱신합니다.
				nodeHierarchyInstance.UpdateByLocalTransform();
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
	auto lowerControllerPath = animator->GetLowerControllerPath();
	bool hasLowerController = std::filesystem::exists(lowerControllerPath);

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
	std::shared_ptr<game_module::AnimatorController> controller = mLoader.Load(controllerPath);
	std::shared_ptr<game_module::AnimatorController> lowerController = hasLowerController ? mLoader.Load(lowerControllerPath) : nullptr;

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

	// 하부 애니메이션 컨트롤러 
	if (hasLowerController)
	{
		for (auto& [stateName, animationStateNode] : lowerController->GetStateMap())
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
	}

	controller->SetAnimator(animator);
	animator->SetController(controller);

	if (hasLowerController)
	{
		lowerController->SetAnimator(animator);
		animator->SetLowerController(lowerController);
	}

	// 스키닝 인스턴싱을 위한 사전 데이터 구성.. 요거 분기 처리할 수 있도록 만들어야댐
	if (animator->GetCreateAnimationTexture())
	{
		assert(nodeHierarchyOrNull != nullptr);
		mGraphics->CreateAnimationTexture(nodeHierarchyOrNull);
	}

	return true;
}