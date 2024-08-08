#include "EffectTrack.h"

#include "GameObject.h"
#include "Scene.h"

namespace fq::game_module
{
	EffectTrack::EffectTrack()
		: Track(ETrackType::EFFECT)
	{
	}
	EffectTrack::~EffectTrack()
	{
	}

	void EffectTrack::Initialize(EffectTrackInfo info, Scene* scene)
	{
		std::shared_ptr<GameObject> object = std::make_shared<GameObject>();

	}

	void EffectTrack::PlayEnter()
	{
	}

	void EffectTrack::PlayOn()
	{
	}

	void EffectTrack::PlayExit()
	{
	}

	void EffectTrack::End()
	{
	}
}
