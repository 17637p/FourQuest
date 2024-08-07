#pragma once

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Event.h"

namespace fq::game_module
{
	class GameObject;
}

namespace fq::game_engine
{
	class GameProcess;

	/// <summary>
	/// UI ���� ���ε� ó���� ����մϴ�.
	/// </summary>
	class UISystem
	{
		using EventHandler = fq::game_module::EventHandler;

	public:
		UISystem();
		~UISystem();

		void Initialize(GameProcess* gameProcess);

		void Update();

		/// <summary>
		/// ���� �ε��Ҷ� �������� ���õ� ���ҽ��� �ε��մϴ�
		/// </summary>
		void OnLoadScene();

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
		/// AddComponent �̺�Ʈ
		/// </summary>
		void AddComponent(const fq::event::AddComponent& event);

		/// <summary>
		/// RemoveCompnoent �̺�Ʈ
		/// </summary>
		void RemoveComponent(const fq::event::RemoveComponent& event);

		/// <summary>
		/// UI���� ���� �̺�Ʈ
		/// </summary>
		void SetUIInfomations(const fq::event::SetUIInfomations& event);

		/// <summary>
		/// Text���� ���� �̺�Ʈ
		/// </summary>
		void SetTextInformation(const fq::event::SetTextInformation& event);

	private:
		void loadImageUI(game_module::GameObject* object);
		void unloadImageUI(game_module::GameObject* object);

		void loadTextUI(game_module::GameObject* object);
		void unloadTextUI(game_module::GameObject* object);

	private:
		GameProcess* mGameProcess;

		EventHandler mOnLoadSceneHandler;
		EventHandler mOnUnloadSceneHandler;
		EventHandler mOnAddGameObjectHandler;
		EventHandler mDestroyedGameObjectHandler;
		EventHandler mAddComponentHandler;
		EventHandler mRemoveComponentHandler;
		EventHandler mSetUIInfomationsHandler;
		EventHandler mSetTextInformationHandler;
		EventHandler mSetScreenSizeHandler;
		EventHandler mSetViewportSizeHandler;

		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mViewporWidth;
		UINT mViewportHeight;

		bool mbIsGameLoaded;
	};



}