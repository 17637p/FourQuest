#include "ObjectAnimationTrack.h"

#include "Scene.h"
#include "Animator.h"
#include "AnimatorController.h"

namespace fq::game_module
{
	ObjectAnimationTrack::ObjectAnimationTrack()
		: Track(ETrackType::ANIMAITON)
	{
	}
	ObjectAnimationTrack::~ObjectAnimationTrack()
	{
	}

	void ObjectAnimationTrack::Initialize(ObjectAnimationInfo info, Scene* scene)
	{
		auto object = scene->GetObjectByName(info.targetObjectName);
	}

	void ObjectAnimationTrack::PlayEnter()
	{
		
	}

	void ObjectAnimationTrack::PlayOn()
	{
	}

	void ObjectAnimationTrack::PlayExit()
	{
	}

	void ObjectAnimationTrack::End()
	{
	}
}
