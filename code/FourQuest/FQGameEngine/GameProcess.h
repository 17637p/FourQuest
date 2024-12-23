#pragma once

#include <memory>

namespace fq::game_module
{
	class SceneManager;
	class EventManager;
	class InputManager;
	class TimeManager;
	class PrefabManager;
	class SoundManager; 
	class ScreenManager;
}

namespace fq::graphics
{
	class IFQGraphics;
}

namespace fq::physics
{
	class IFQPhysics;
}

namespace fq::game_engine
{
	class WindowSystem;
	class CameraSystem;
	class RenderingSystem;
	class PhysicsSystem;
	class LightSystem;
	class SoundSystem;
	class AnimationSystem;
	class ParticleSystem;
	class DecalSystem;
	class UISystem;
	class PathFindingSystem;
	class TrailSystem;
	class LightProbeSystem;
	class LoadingSystem;
	class ResourceSystem;
	class StateEventSystem;
}

namespace fq::game_engine
{
	/// <summary>
	/// GameProcess에서 사용되는 클래스 집합
	/// </summary>
	class GameProcess
	{
	public:
		GameProcess();
		~GameProcess();

		// GameModule
		std::unique_ptr<fq::game_module::InputManager> mInputManager;
		std::unique_ptr<fq::game_module::TimeManager>  mTimeManager;
		std::unique_ptr<fq::game_module::SceneManager> mSceneManager;
		std::unique_ptr<fq::game_module::EventManager> mEventManager;
		std::unique_ptr<fq::game_module::PrefabManager> mPrefabManager;
		std::unique_ptr<fq::game_module::SoundManager> mSoundManager;
		std::unique_ptr<fq::game_module::ScreenManager> mScreenManager;

		// Graphics
		fq::graphics::IFQGraphics* mGraphics;
		
		// Physics
		fq::physics::IFQPhysics* mPhysics;

		// System
		std::unique_ptr<WindowSystem> mWindowSystem;
		std::unique_ptr<RenderingSystem> mRenderingSystem;
		std::unique_ptr<CameraSystem> mCameraSystem;
		std::unique_ptr<PhysicsSystem> mPhysicsSystem;
		std::unique_ptr<LightSystem> mLightSystem;
		std::unique_ptr<SoundSystem> mSoundSystem;
		std::unique_ptr<AnimationSystem> mAnimationSystem;
		std::unique_ptr<ParticleSystem> mParticleSystem;
		std::unique_ptr<DecalSystem> mDecalSystem;
		std::unique_ptr<TrailSystem> mTrailSystem;
		std::unique_ptr<UISystem> mUISystem;
		std::unique_ptr<PathFindingSystem> mPathFindgingSystem;
		std::unique_ptr<LightProbeSystem> mLightProbeSystem;
		std::unique_ptr<LoadingSystem> mLoadingSystem;
		std::unique_ptr<ResourceSystem> mResourceSystem;
		std::unique_ptr<StateEventSystem> mStateEventSystem;
	};
}