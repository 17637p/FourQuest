#pragma once

#include "../FQGameModule/Component.h"
#include "../FQGameModule/PrefabResource.h"

namespace fq::client
{
	class GameManager;

	class PvPManager : public game_module::Component
	{
	public:
		void OnUpdate(float dt) override;
		void OnStart() override;
		void SpawnArmour();

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		unsigned int checkLivePlayer();
		unsigned int checkJoinPlayer();

	private:
		GameManager* mGameManager;

		std::vector<game_module::PrefabResource> mArmours;
		std::string mEndSeuqenceName; 

		int mMaxSpawnAromour;
		int mMinSpawnAromour;
		float mMaxSpawnRange;
		float mMinSpawnRange;
		float mElapsedTime;
		bool mbIsEndGame;

		friend void RegisterMetaData();
	};

}