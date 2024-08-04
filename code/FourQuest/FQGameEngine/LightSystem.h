#pragma once

#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class GameObject;
	class Transform;
	class Light;
}

namespace fq::game_engine
{
	class GameProcess;

	class LightSystem
	{
		using EventHandler = fq::game_module::EventHandler;
	public:
		LightSystem();
		~LightSystem();

		/// <summary>
		/// 라이트 시스템을 초기화합니다
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// 라이트를 업데이트 합니다
		/// </summary>
		void Update();

		/// <summary>
		/// 씬을 로드할때 랜더링에 관련된 리소스를 로드합니다
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

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
		/// 라이트 설정 이벤트
		/// </summary>
		void SetLightType(const fq::event::SetLightType& event);

		/// <summary>
		/// AddComponent 이벤트
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent 이벤트
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		/// <summary>
		/// SetLightShadow Event
		/// </summary>
		void SetLightShadow(const fq::event::SetLightShadow& event);

	private:
		void addLight(fq::game_module::GameObject* object);
		void updateLight(fq::game_module::Light& light, fq::game_module::Transform& transform);
		void deleteLight(fq::game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;
		fq::game_module::Scene* mScene;

		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mSetLightInfoHandler;
		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mSetLightShadowHandler;

		bool mbIsGameLoaded;
	};


}