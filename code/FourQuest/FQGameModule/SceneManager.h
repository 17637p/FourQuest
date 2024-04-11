#pragma once

#include <memory>
#include <string>

namespace fq::game_module
{
	class Scene;

	class InputManager;
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
		void Initialize(const std::string& startSceneName, EventManager* eventMgr, InputManager* inputMgr);

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