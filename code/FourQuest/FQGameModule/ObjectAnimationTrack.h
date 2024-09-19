#pragma once

#include "Sequence.h"
#include "Track.h"

namespace fq::game_module
{
	class ObjectAnimationTrack : public Track
	{
	public:
		ObjectAnimationTrack();
		~ObjectAnimationTrack();

		bool Initialize(const ObjectAnimationInfo& info, Scene* scene, std::vector<std::shared_ptr<fq::graphics::IAnimation>> animationContainer);

		const std::string& GetTargetObjectName() { return mTargetObjectName; }
		void SetTargetObjectName(const std::string& objectName) { mTargetObjectName = objectName; }
		const std::string& GetPrevAnimationPath() { return mPrevAnimationPath; }
		void SetPrevAnimationPath(const std::string& animationPath) { mPrevAnimationPath = animationPath; }
		const std::vector<AnimationTrackKey> GetAnimationTrackKey() { return mAnimationTrackKeys; }
		void SetAnimationTrackKey(std::vector<AnimationTrackKey> animationTrackKeys) { mAnimationTrackKeys = animationTrackKeys; }

	private:
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;
		virtual void End() override;

	private:
		std::string mTargetObjectName;
		std::vector<AnimationTrackKey> mAnimationTrackKeys;
		std::string mPrevAnimationPath;

		std::weak_ptr<GameObject> mTargetObject;
		std::vector<std::shared_ptr<fq::graphics::IAnimation>> mAnimationContainer;
	};
}