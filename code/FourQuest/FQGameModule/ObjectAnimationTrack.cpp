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

	bool ObjectAnimationTrack::Initialize(const ObjectAnimationInfo& info, Scene* scene)
	{
		auto object = scene->GetObjectByName(info.targetObjectName);

		return true;
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
