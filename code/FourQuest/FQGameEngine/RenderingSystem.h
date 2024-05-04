#pragma once

#include <set>
#include <string>

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// 랜더링관련 바인딩 처리를 담당합니다
	/// </summary>
	class RenderingSystem
	{
		using ModelPath = std::string;
		using EventHandler = fq::game_module::EventHandler;
	public:
		RenderingSystem();
		~RenderingSystem();

		/// <summary>
		/// 랜더링 시스템을 초기화합니다
		/// </summary>
		void Initialize(GameProcess* gameProcess);

		void Update(float dt);

		/// <summary>
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void OnLoadScene();

		/// <summary>
		/// 씬을 언로드할때 랜더링에 관련된 리소스를 언로드합니다.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// 게임오브젝트 추가 이벤트
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// 게임오브젝트 삭제 이벤트
		/// </summary>
		void OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event);

		/// <summary>
		/// AddComponent 이벤트
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent 이벤트
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		/// <summary>
		/// 모델이 로드된지 확인합니다
		/// </summary>
		bool IsLoadedModel(const ModelPath& path);

	private:
		void loadStaticMeshRenderer(fq::game_module::GameObject* object);
		void unloadStaticMeshRenderer(fq::game_module::GameObject* object);
		void loadSkinnedMeshRenderer(fq::game_module::GameObject* object);
		void unloadSkinnedMeshRenderer(fq::game_module::GameObject* object);

		void loadModel(ModelPath path);
		void unloadAllModel();

	private:
		GameProcess* mGameProcess;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;

		bool mbIsGameLoaded;

		std::set<ModelPath> mLoadModels;
	};
}