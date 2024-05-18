#include "AnimationSystem.h"

#include "GameProcess.h"

fq::game_engine::AnimationSystem::AnimationSystem()
	:mGameProcess(nullptr)
	, mScene(nullptr)
	, mGraphics(nullptr)
	, mChangeAnimationStateHandler{}
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

	// 2. OnStateEnter/Exit CallBack


	// 3. AnimationÀ» Àû¿ë
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

	mScene->ViewComponents<SkinnedMeshRenderer, Animator>(
		[dt](GameObject& object, SkinnedMeshRenderer mesh, Animator& animator)
		{
			float timePos = animator.UpdateAnimation(dt);
			mesh.GetSkinnedMeshObject()->SetAnimationTime(timePos);
		});
}


void fq::game_engine::AnimationSystem::ChangeAnimationState(const fq::event::ChangeAnimationState& event)
{
	auto skinnedMesh = event.object->GetComponent<fq::game_module::SkinnedMeshRenderer>()
		->GetSkinnedMeshObject();

	skinnedMesh->SetAnimationKey(event.enterState);
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

	auto controller = mLoader.Load(controllerPath);
	controller->SetAnimator(animator);
	animator->SetController(controller);

	return true;
}
