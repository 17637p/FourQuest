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
		void Finalize();

		void Update(float dt);

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

		/// <summary>
		/// SpriteAnimation���� ���� �̺�Ʈ
		/// </summary>
		void SetSpriteAnimationInformation(const fq::event::SetSpriteAnimationInformation& event);

		/// <summary>
		/// Video���� ���� �̺�Ʈ
		/// </summary>
		void SetVideoInformation(const fq::event::SetVideoInformation& event);

		void LoadImageUI(game_module::GameObject* object);
		void UnloadImageUI(game_module::GameObject* object);

		void LoadTextUI(game_module::GameObject* object);
		void UnloadTextUI(game_module::GameObject* object);

		void LoadSpriteAnimationUI(game_module::GameObject* object);
		void UnloadSpriteAnimationUI(game_module::GameObject* object);

		void LoadVideoUI(game_module::GameObject* object);
		void UnloadVideoUI(game_module::GameObject* object);

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
		EventHandler mSetSpriteInformationHandler;
		EventHandler mSetVideoInformationHandler;
		EventHandler mSetScreenSizeHandler;
		EventHandler mSetViewportSizeHandler;

		UINT mScreenWidth;
		UINT mScreenHeight;
		UINT mViewporWidth;
		UINT mViewportHeight;

		bool mbIsGameLoaded;
	};



}