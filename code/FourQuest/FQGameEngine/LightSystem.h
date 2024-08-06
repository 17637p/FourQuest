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
		/// ����Ʈ �ý����� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize(GameProcess* game);

		/// <summary>
		/// ����Ʈ�� ������Ʈ �մϴ�
		/// </summary>
		void Update();

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene(const fq::event::OnLoadScene event);

		/// <summary>
		/// ���� ��ε��Ҷ� �������� ���õ� ���ҽ��� ��ε��մϴ�.
		/// </summary>
		void OnUnLoadScene();

		/// <summary>
		/// ���ӿ�����Ʈ �߰� �̺�Ʈ
		/// </summary>
		void OnAddGameObject(const fq::event::AddGameObject& event);

		/// <summary>
		/// ���ӿ�����Ʈ ���� �̺�Ʈ
		/// </summary>
		void OnDestroyedGameObject(const fq::event::OnDestoryedGameObject& event);

		/// <summary>
		/// ����Ʈ ���� �̺�Ʈ
		/// </summary>
		void SetLightType(const fq::event::SetLightType& event);

		/// <summary>
		/// AddComponent �̺�Ʈ
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent �̺�Ʈ
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