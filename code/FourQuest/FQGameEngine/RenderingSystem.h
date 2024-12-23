#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>

#include "../FQGameModule/EventHandler.h"
#include "../FQGameModule/Event.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::graphics
{
	class IImageObject;
}

namespace fq::game_engine
{
	class GameProcess;
	class ResourceSystem;

	/// <summary>
	/// 랜더링관련 바인딩 처리를 담당합니다
	/// </summary>
	class RenderingSystem
	{
		using Path = std::string;
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
		/// 애니메이션을 작동합니다.
		/// </summary>
		void WriteAnimation(const fq::event::WriteAnimation& event);

		/// <summary>
		// 현재 활성화된 UI On/Off 이벤트
		/// </summary>
		void OnUIRender(const fq::event::UIRender& event);

		unsigned int GetModelKey(const Path& modelPath, const Path& texturePath = "")const;

	private:
		void loadStaticMeshRenderer(fq::game_module::GameObject* object);
		void unloadStaticMeshRenderer(fq::game_module::GameObject* object);

		void loadSkinnedMeshRenderer(fq::game_module::GameObject* object);
		void unloadSkinnedMeshRenderer(fq::game_module::GameObject* object);

		void loadTerrain(fq::game_module::GameObject* object);
		void unloadTerrain(fq::game_module::GameObject* object);

		void loadAnimation(fq::game_module::GameObject* object);
		void unloadAnimation(fq::game_module::GameObject* object);

		void loadUVAnimation(fq::game_module::GameObject* object);
		void loadSequenceAnimation(fq::game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;
		ResourceSystem* mResourceSystem;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mWriteAnimationHandler;
		EventHandler mSetViewportSizeHandler;
		EventHandler mUpdateMaterialInfosHandler;
		EventHandler mDebugDrawLayHanlder;
		EventHandler mUIRenderHandler;

		bool mbIsGameLoaded;
		DirectX::SimpleMath::Matrix mPlaneMatrix;

		std::map<fq::game_module::GameObject*, std::vector<size_t>> mOnImageUIRenderEventActivatedObjects;
		std::set<fq::game_module::GameObject*> mOnTextUIRenderEventActivatedObjects;
		std::set<fq::game_module::GameObject*> mOnSpriteUIRenderEventActivatedObjects;
	};
}