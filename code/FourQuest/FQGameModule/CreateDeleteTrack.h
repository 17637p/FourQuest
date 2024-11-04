#pragma once

#include "SequenceStructure.h"
#include "Track.h"
#include "PrefabResource.h"

namespace fq::game_module
{
	class CreateDeleteTrack :public Track
	{
	public:
		CreateDeleteTrack();
		~CreateDeleteTrack();

		bool Initialize(const CreateDeleteTrackInfo& info, Scene* scene, const std::vector<PrefabResource>& reosource);

	private:
		virtual void End() override;
		virtual void PlayEnter() override;
		virtual void PlayOn() override;
		virtual void PlayExit() override;

	private:
		Scene* mScene;
		ECreateDeleteMode mMode;
		PrefabResource mCreateResouce; 
		std::string mDeleteName;

		std::shared_ptr<GameObject> mObject;
	};


}