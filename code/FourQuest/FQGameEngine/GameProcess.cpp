#include "GameProcess.h"

#include "../FQGameModule/GameModule.h"

#include "WindowSystem.h"
#include "ModelSystem.h"
#include "RenderingSystem.h"
#include "CameraSystem.h"
#include "PhysicsSystem.h"
#include "LightSystem.h"
#include "SoundSystem.h"
#include "AnimationSystem.h"
#include "ParticleSystem.h"
#include "DecalSystem.h"
#include "TrailSystem.h"
#include "UISystem.h"
#include "PathFindingSystem.h"
#include "LightProbeSystem.h"
#include "LoadingSystem.h"
#include "ResourceSystem.h"


fq::game_engine::GameProcess::GameProcess()
	: mInputManager(std::make_unique<fq::game_module::InputManager>())
	, mTimeManager(std::make_unique<fq::game_module::TimeManager>())
	, mSceneManager(std::make_unique<fq::game_module::SceneManager>())
	, mEventManager(std::make_unique<fq::game_module::EventManager>())
	, mPrefabManager(std::make_unique<fq::game_module::PrefabManager>())
	, mSoundManager(std::make_unique<fq::game_module::SoundManager>())
	, mScreenManager(std::make_unique<fq::game_module::ScreenManager>())
	, mWindowSystem(std::make_unique<WindowSystem>())
	, mRenderingSystem(std::make_unique<RenderingSystem>())
	, mCameraSystem(std::make_unique<CameraSystem>())
	, mPhysicsSystem(std::make_unique<PhysicsSystem>())
	, mLightSystem(std::make_unique<LightSystem>())
	, mSoundSystem(std::make_unique<SoundSystem>())
	, mAnimationSystem(std::make_unique<AnimationSystem>())
	, mParticleSystem(std::make_unique<ParticleSystem>())
	, mDecalSystem(std::make_unique<DecalSystem>())
	, mTrailSystem(std::make_unique<TrailSystem>())
	, mUISystem(std::make_unique<UISystem>())
	, mPathFindgingSystem(std::make_unique<PathFindingSystem>(this))
	, mLightProbeSystem(std::make_unique<LightProbeSystem>(this))
	, mLoadingSystem(std::make_unique<LoadingSystem>())
	, mResourceSystem(std::make_unique<ResourceSystem>())
	, mGraphics(nullptr)
	, mPhysics(nullptr)
{}

fq::game_engine::GameProcess::~GameProcess()
{}

