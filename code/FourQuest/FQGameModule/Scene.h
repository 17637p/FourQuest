#pragma once

#include "GameObjectIterator.h"
#include "GameObjectView.h"
#include "ComponentIterator.h"
#include "ComponentView.h"
#include "GameObject.h"


namespace fq::game_module
{
	class GameObject;
	class EventManager;
	class InputManager;
	class PrefabManager;
	class ScreenManager;

	class Scene
	{
	public:
		Scene();
		~Scene();

		/// <summary>
		/// Scene�� �����͸� �ε��Ѵ�
		/// </summary>
		void Initialize(std::string sceneName
			, EventManager* eventMgr
			, InputManager* inputMgr
			, PrefabManager* prefabMgr
			, ScreenManager* screenMgr);

		/// <summary>
		/// EventManager�� ��ȯ�մϴ� 
		/// </summary>
		EventManager* GetEventManager()const { return mEventManager; }

		/// <summary>
		/// InputManager�� ��ȯ�մϴ�
		/// </summary>
		InputManager* GetInputManager()const { return mInputManager; }

		/// <summary>
		/// PrefabManager�� ��ȯ�մϴ� 
		/// </summary>
		PrefabManager* GetPrefabManager()const { return mPrefabManager; }

		ScreenManager* GetScreenManager()const { return mScreenManager; }

		/// <summary>
		/// ���� ������ ������Ʈ �����մϴ�
		/// 
		/// </summary>
		void CleanUp(bool isRemoveComponents = true);

		/// <summary>
		/// Scene ���ӿ�����Ʈ�� �߰��մϴ�
		/// ������Ʈ�� �ڽİ������� ��� �߰��մϴ�.
		/// </summary>
		/// <param name="object">������Ʈ</param>
		void AddGameObject(std::shared_ptr<GameObject> object);

		/// <summary>
		/// Scene ������ ���ӿ�����Ʈ�� �����մϴ�
		/// </summary>
		/// <param name="object"></param>
		void DestroyGameObject(GameObject* object);

		/// <summary>
		/// Scene������ ��� ���� ������Ʈ�� �����մϴ�
		/// </summary>
		void DestroyAll();

		/// <summary>
		/// Scene�� ������ ������Ʈ ���� ��ȯ
		/// </summary>
		/// <returns>������Ʈ ����</returns>
		size_t GetObjectSize()const { return mObjects.size(); }

		/// <summary>
		/// ���� ���� �̸��� ��ȯ
		/// </summary>
		/// <returns>���� �̸�</returns>
		std::string GetSceneName()const { return mSceneName; }

		/// <summary>
		/// �ε����� �ش��ϴ� ������Ʈ ��ȯ
		/// !! GameObject ID�� �ƴմϴ� !!
		/// </summary>
		/// <param name="index">�ε���</param>
		/// <returns>index�� �ʰ��Ѱ�� nulllptr �̰� �ƴϸ� �������� ���� ������Ʈ</returns>
		GameObject* GetObjectByIndex(size_t index);

		/// <summary>
		/// ������Ʈ ID�� �ش��ϴ� ������Ʈ ��ȯ
		/// </summary>
		/// <param name="id">������Ʈ�� ���� ID</param>
		/// <returns>ã�����Ѱ�� nullptr ��ȯ�ϰ� �ƴϸ� id�� �����ϴ� ��ü��ȯ</returns>
		std::shared_ptr<GameObject> GetObjectByID(unsigned int id);

		/// <summary>
		/// Scene���� ������Ʈ �̸����� Ž���ؼ� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<GameObject> GetObjectByName(std::string name);

		/// <summary>
		/// Scene�� ���ӿ�����Ʈ�� ��ȸ�ϴ� view ��ü�� ��ȯ 
		/// </summary>
		/// <param name="bIsIncludeToBeDestroyed">���������� ������Ʈ�� ���� ����</param>
		/// <returns>���ӿ�����Ʈ view ��ü</returns>
		internal::GameObjectView GetObjectView(bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// ���ӿ�����Ʈ ��ȸ�ϸ鼭 viewFunc�� ȣ���Ѵ�.
		/// </summary>
		/// <param name="viewFunc">ȣ���� �Լ�</param>
		/// <param name="bIsIncludeToBeDestroyed">���� ������ ������Ʈ ���� ����</param>
		void ViewObjects(std::function<void(GameObject&)> viewFunction
			, bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// �������ڿ� �ش��ϴ� ������Ʈ Ÿ�Ե��� ���� 
		/// ������Ʈ���� �����Ͽ� ��ȸ�ϴ� View��ü ��ȯ
		/// </summary>
		/// <typeparam name="...Types">������ ������Ʈ Ÿ�Ե�</typeparam>
		/// <param name="bIsIncludeToBeDestroyed">���� ���� ������Ʈ ���� ����</param>
		/// <returns>��ȸ������ View��ü ��ȯ</returns>
		template <typename... Types>
		internal::ComponentView<Types...> GetComponentView(bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// �������ڿ� �ش��ϴ� ������Ʈ Ÿ�Ե��� ���� 
		/// ������Ʈ���� �����Ͽ� viewFunction�� ȣ��
		/// </summary>
		/// <typeparam name="...Types">������ ������Ʈ Ÿ��</typeparam>
		/// <param name="viewFunction">������ ��ü���� ������ �Լ�</param>
		/// <param name="bIsIncludeToBeDestroyed">���� ���� ������Ʈ ���� ����</param>
		template <typename... Types>
		void ViewComponents(typename std::common_type_t<std::function<void(GameObject&, Types& ...)>> viewFunction,
			bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// ���� ���۵Ǿ����� ��ȯ�մϴ�
		/// </summary>
		bool IsStartScene() const { return mIsStartScene; }

	private:
		void destroyChild(GameObject* object);

		/// <summary>
		/// �߰��� ������Ʈ�� ���� ó�����մϴ�
		/// </summary>
		void processPedingObject();

	private:
		std::string mSceneName;
		std::vector<std::shared_ptr<GameObject>> mObjects;
		std::vector <std::shared_ptr<GameObject>> mPedingObjects;

		InputManager* mInputManager;
		EventManager* mEventManager;
		PrefabManager* mPrefabManager;
		ScreenManager* mScreenManager;

		bool mIsStartScene;

		friend class SceneManager;
	};


	template<typename ...Types>
	inline internal::ComponentView<Types...> Scene::GetComponentView(bool bIsIncludeDestroyed)
	{
		internal::ComponentIterator<Types...> beginIter(this, 0, false, bIsIncludeDestroyed);
		internal::ComponentIterator<Types...> endIter(this, GetObjectSize(), true, bIsIncludeDestroyed);

		return internal::ComponentView<Types...>(beginIter, endIter);
	}

	template<typename ...Types>
	inline void Scene::ViewComponents(typename std::common_type_t<std::function<void(GameObject&, Types& ...)>> viewFunction
		, bool bIsIncludeDestroyed)
	{
		for (GameObject& object : GetComponentView<Types ...>(bIsIncludeDestroyed))
		{
			viewFunction(object, *(object.template GetComponent<Types>()) ...);
		}
	}

}