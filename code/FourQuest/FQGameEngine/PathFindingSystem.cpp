#include "PathFindingSystem.h"

#include "../FQGameModule/Scene.h"
#include "../FQGameModule/NavigationAgent.h"

#include "NavigationMesh.h"
#include "GameProcess.h"

fq::game_engine::PathFindingSystem::PathFindingSystem(GameProcess* tempProcess)
	:mHasNavigationMesh(false),
	mIsLoadedScene(false),
	mBuildSettings{ new BuildSettings },
	mBuilder{ new NavigationMeshBuilder(tempProcess) },
	mGameProcess(tempProcess)
{
}

fq::game_engine::PathFindingSystem::~PathFindingSystem()
{
	delete mBuildSettings;
	delete mBuilder;
}

void fq::game_engine::PathFindingSystem::BuildNavigationMesh(fq::game_module::Scene* scene)
{
	mBuilder->BuildNavigationMesh(scene, *mBuildSettings);
	mHasNavigationMesh = true;

	for (const auto& agent : mAgents)
	{
		agent->RegisterNavigationField(this);
	}
}

std::vector<DirectX::SimpleMath::Vector3> fq::game_engine::PathFindingSystem::GetNavMeshVertices()
{
	return mBuilder->GetNavMeshVertices();
}

fq::game_engine::BuildSettings* fq::game_engine::PathFindingSystem::GetBuildingSettrings()
{
	return mBuildSettings;
}

void fq::game_engine::PathFindingSystem::Update(float dt)
{
	if (!mIsStartScene)
	{
		return;
	}

	if (mHasNavigationMesh)
	{
		//if (mGameProcess->mInputManager->GetKeyState(EKey::T) == EKeyState::Tap)
		//{
		//	for (auto& agent : mAgents)
		//	{
		//		agent->MoveTo({ -1.631, 0.884, -14.869 });
		//	}
		//}

		mBuilder->Update(dt);

		for (auto& agent : mAgents)
		{
			fq::game_module::Transform* agentT = agent->GetGameObject()->GetComponent<fq::game_module::Transform>();

			// 등록 안된 Agent
			if (agent->GetAgentIndex() == -1)
			{
				continue;
			}

			auto npos = mBuilder->GetCrowd()->getAgent(agent->GetAgentIndex())->npos;
			agentT->SetLocalPosition({ npos[0], npos[1], npos[2] });
		}
	}
}

dtNavMeshQuery* fq::game_engine::PathFindingSystem::GetNavQuery() const
{
	return mBuilder->GetNavQuery();
}

int fq::game_engine::PathFindingSystem::AddAgentToCrowd(DirectX::SimpleMath::Vector3 pos, dtCrowdAgentParams* agentParams)
{
	return mBuilder->AddAgent(pos, agentParams);
}

void fq::game_engine::PathFindingSystem::Initialize(GameProcess* game)
{
	auto& eventMgr = game->mEventManager;
	mScene = game->mSceneManager->GetCurrentScene();

	mOnLoadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnLoadScene>(this, &PathFindingSystem::OnLoadScene);
	mOnUnloadSceneHandler = eventMgr->
		RegisterHandle<fq::event::OnUnloadScene>(this, &PathFindingSystem::OnUnloadScene);
	mAddComponentHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddComponent>(this, &PathFindingSystem::OnAddComponent);
	mRemoveComponentHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::RemoveComponent>(this, &PathFindingSystem::OnRemoveComponent);
	mOnAddGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::AddGameObject>(this, &PathFindingSystem::OnAddGameObject);
	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::OnDestoryedGameObject>(this, &PathFindingSystem::OnDestroyedGameObject);
	mDestroyedGameObjectHandler = mGameProcess->mEventManager->
		RegisterHandle<fq::event::StartScene>(this, &PathFindingSystem::OnStartScene);

	mAgentID = entt::resolve<fq::game_module::NavigationAgent>().id();
}

dtCrowd* fq::game_engine::PathFindingSystem::GetCrowd()
{
	return mBuilder->GetCrowd();
}

void fq::game_engine::PathFindingSystem::OnLoadScene(const fq::event::OnLoadScene event)
{
	for (auto& object : mScene->GetObjectView())
	{
		auto navMeshAgent = object.GetComponent<fq::game_module::NavigationAgent>();
		if (navMeshAgent != nullptr)
		{
			mAgents.push_back(navMeshAgent);
			navMeshAgent->CreateAgentData();
			if (mHasNavigationMesh)
			{
				navMeshAgent->RegisterNavigationField(this);
			}
		}
	}
	mIsLoadedScene = true;
}

void fq::game_engine::PathFindingSystem::OnUnloadScene()
{
	mIsLoadedScene = false;
	mIsStartScene = false;
	//mAgents.clear();
}

void fq::game_engine::PathFindingSystem::OnAddComponent(const fq::event::AddComponent& event)
{
	if (!mIsLoadedScene)
	{
		return;
	}

	if (event.id == mAgentID)
	{
		fq::game_module::NavigationAgent* agent = static_cast<fq::game_module::NavigationAgent*>(event.component);
		mAgents.push_back(agent);
		agent->CreateAgentData();
		if (mHasNavigationMesh)
		{
			agent->RegisterNavigationField(this);
		}
	}
}

void fq::game_engine::PathFindingSystem::OnRemoveComponent(const fq::event::RemoveComponent& event)
{
	if (event.id == mAgentID)
	{
		fq::game_module::NavigationAgent* agent = static_cast<fq::game_module::NavigationAgent*>(event.component);
		agent->DeleteAgentData();
		mAgents.erase(std::remove(mAgents.begin(), mAgents.end(), agent));
	}
}

void fq::game_engine::PathFindingSystem::OnAddGameObject(const fq::event::AddGameObject& event)
{
	if (!mIsLoadedScene)
	{
		return;
	}

	fq::game_module::NavigationAgent* agent = event.object->GetComponent<fq::game_module::NavigationAgent>();
	if (agent != nullptr)
	{
		mAgents.push_back(agent);
		agent->CreateAgentData();
		if (mHasNavigationMesh)
		{
			agent->RegisterNavigationField(this);
		}
	}
}

void fq::game_engine::PathFindingSystem::OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event)
{
	fq::game_module::NavigationAgent* agent = event.object->GetComponent<fq::game_module::NavigationAgent>();
	if (agent != nullptr)
	{
		agent->DeleteAgentData();
		mAgents.erase(std::remove(mAgents.begin(), mAgents.end(), agent));
	}
}

bool fq::game_engine::PathFindingSystem::HasNavigationMesh()
{
	return mHasNavigationMesh;
}

void fq::game_engine::PathFindingSystem::OnStartScene()
{
	mIsStartScene = true;
}

void fq::game_engine::PathFindingSystem::SaveNavMesh(std::string& fileName)
{
	mBuilder->SaveNavMesh(fileName);
}

void fq::game_engine::PathFindingSystem::LoadNavMesh(std::string& fileName)
{
	mBuilder->LoadNavMesh(fileName, *mBuildSettings);
}
