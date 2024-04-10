#pragma once

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;

	class EventManager;

	/// <summary>
	/// ���� �����ϴ� Ŭ���� 
	/// </summary>
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		/// <summary>
		/// ���� ���õ� �ʱ�ȭ�� �մϴ�
		/// </summary>
		/// <param name="startSceneName">���� ���� �̸�</param>
		/// <param name="eventMgr">�̺�Ʈ �Ŵ���</param>
		void Initialize(const std::string& startSceneName, EventManager* eventMgr);
		
		/// <summary>
		/// ���� ���õ� ����ó���� �մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� �����մϴ�.
		/// </summary>
		/// <param name="sceneName">���� ���� �̸�</param>
		void ChangeScene(const std::string& nextSceneName);


		/// <summary>
		/// ���� ���� ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ��</returns>
		Scene* GetCurrentScene()const { return mCurrentScene.get(); };

	private:
		void loadScene();

	private:
		std::unique_ptr<Scene> mCurrentScene;
		EventManager* mEventManager;
	};


}