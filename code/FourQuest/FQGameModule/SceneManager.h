#pragma once

#include <memory>
#include <string>

#include "EventHandler.h"
#include "Event.h"

namespace fq::game_module
{
	class Scene;

	class PrefabManager;
	class InputManager;
	class EventManager;
	class ScreenManager;
	class TimeManager;

	/// <summary>
	/// ���� �����ϴ� Ŭ���� 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		/// <summary>
		/// ���Ŵ����� �ʱ�ȭ�� �մϴ�
		/// </summary>
		/// <param name="startSceneName">���� ���� �̸�</param>
		/// <param name="eventMgr">�̺�Ʈ �Ŵ���</param>
		void Initialize(const std::string& startSceneName
			, EventManager* eventMgr
			, InputManager* inputMgr
			, PrefabManager* prefabMgr
			, ScreenManager* screenMgr
			, TimeManager* timeMgr);

		/// <summary>
		/// �� �Ŵ����� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� �ε��մϴ�
		/// </summary>
		void LoadScene();

		/// <summary>
		/// ���� ��ε��մϴ�
		/// </summary>
		void UnloadScene();

		/// <summary>
		/// Scene�� �����մϴ�
		/// </summary>
		void StartScene();

		/// <summary>
		/// ���� ó���� ���� ���������� ������Ʈ
		/// </summary>
		/// <param name="dt"></param>
		void FixedUpdate(float dt);

		/// <summary>
		/// �� ������Ʈ
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

		/// <summary>
		/// ���� ��� ������Ʈ ������Ʈ ���Ŀ� ȣ��
		/// </summary>
		/// <param name="dt"></param>
		void LateUpdate(float dt);

		/// <summary>
		/// ���� �����մϴ�.
		/// </summary>
		void ChangeScene();

		/// <summary>
		/// ��ó�� �̺�Ʈ�� ó���մϴ�
		/// </summary>
		void PostUpdate();

		/// <summary>
		/// ���� ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ��</returns>
		Scene* GetCurrentScene()const { return mCurrentScene.get(); };

		/// <summary>
		/// ������ ���Ῡ�θ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>����Ǹ� true, �׷��������� false</returns>
		bool IsEnd() const { return mbIsEnd; }

		/// <summary>
		/// �� ���� ��û�� ���Ŵ������� �����ϴ�
		/// </summary>
		/// <param name="event">��û�� �ʿ��� ����</param>
		void RequestChangeScene(fq::event::RequestChangeScene event);
		
		/// <summary>
		/// ���� ���� ��û�� �����ϴ�
		/// </summary>
		/// <param name="event">��û�� �ʿ��� ����</param>
		void RequestExitGame(fq::event::RequestExitGame event);
		
		bool IsChangeScene()const;

	private:
		std::unique_ptr<Scene> mCurrentScene;
		
		EventManager* mEventManager;
		PrefabManager* mPrefabManager;
		TimeManager* mTimeManager;

		EventHandler mRequestChangeSceneHandler;
		EventHandler mRequestExitGameHadler;

		std::string mNextSceneName;
		bool mbIsInvokeStartScene;
		bool mbIsEnd;
	};


}