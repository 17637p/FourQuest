#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{ 
	class Scene; 
	class NavigationAgent;
}

class dtNavMeshQuery;
class dtCrowdAgentParams;
class dtCrowd;

namespace fq::game_engine
{
	class GameProcess;

	class NavigationMeshBuilder;
	struct BuildSettings;

	class PathFindingSystem
	{
	public:
		PathFindingSystem(GameProcess* tempProcess);
		~PathFindingSystem();

		BuildSettings* GetBuildingSettrings();
		void BuildNavigationMesh(fq::game_module::Scene* scene);

		void Initialize(GameProcess* game);
		void Update(float dt);

		// agent
		dtNavMeshQuery* GetNavQuery() const;
		int AddAgent(DirectX::SimpleMath::Vector3 pos, dtCrowdAgentParams* agentParams);
		dtCrowd* GetCrowd();

		// Debug
		std::vector<DirectX::SimpleMath::Vector3> GetNavMeshVertices();

		// Event Process
		void OnLoadScene(const fq::event::OnLoadScene event);
		void OnUnloadScene();
		void OnCleanUpScene();
		void OnAddComponent(const fq::event::AddComponent& event);

	private:
		// Save
		//bool duDumpPolyMeshToObj(struct rcPolyMesh& pmesh, duFileIO* io);
		//bool duDumpPolyMeshDetailToObj(struct rcPolyMeshDetail& dmesh, duFileIO* io);

	private:
		// Agent
		std::vector<fq::game_module::NavigationAgent*> mAgents;

		// Navigation 
		BuildSettings* mBuildSettings;
		NavigationMeshBuilder* mBuilder;

		// 각종 엔진 처리
		fq::game_engine::GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		fq::game_module::EventHandler mOnLoadSceneHandler;
		fq::game_module::EventHandler mOnUnloadSceneHandler;
		fq::game_module::EventHandler mOnCleanUpSceneHandler;

		entt::id_type mAgentID;

		fq::game_module::EventHandler mAddComponentHandler;
		fq::game_module::EventHandler mRemoveComponentHandler;
		fq::game_module::EventHandler mOnAddGameObjectHandler;
		fq::game_module::EventHandler mDestroyedGameObjectHandler;
	};
}

