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

	class Scene
	{
	public:
		Scene();
		~Scene();

		/// <summary>
		/// Scene�� �����͸� �ε��Ѵ�
		/// </summary>
		void Initialize(std::string sceneName, EventManager* eventMgr, InputManager* inputMgr);

		/// <summary>
		/// ���� ������ ������Ʈ �����մϴ�
		/// </summary>
		void CleanUp();

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
		std::shared_ptr<GameObject> GetObjectByIndex(size_t index);

		/// <summary>
		/// ������Ʈ ID�� �ش��ϴ� ������Ʈ ��ȯ
		/// </summary>
		/// <param name="id">������Ʈ�� ���� ID</param>
		/// <returns>ã�����Ѱ�� nullptr ��ȯ�ϰ� �ƴϸ� id�� �����ϴ� ��ü��ȯ</returns>
		std::shared_ptr<GameObject> GetObjectByID(unsigned int id);

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

	private:
		void destroyChild(GameObject* object);

	private:
		std::string mSceneName;
		std::vector<std::shared_ptr<GameObject>> mObjects;

		InputManager* mInputManager;
		EventManager* mEventManager;

		friend class SceneManager;
	};


	template<typename ...Types>
	inline internal::ComponentView<Types...> Scene::GetComponentView(bool bIsIncludeToBeDestroyed)
	{
		internal::ComponentIterator<Types...> beginIter(this, 0, false, bIsIncludeToBeDestroyed);
		internal::ComponentIterator<Types...> endIter(this, GetObjectSize(), true, bIsIncludeToBeDestroyed);

		return internal::ComponentView<Types...>(beginIter, endIter);
	}

	template<typename ...Types>
	inline void Scene::ViewComponents(typename std::common_type_t<std::function<void(GameObject&, Types& ...)>> viewFunction
		, bool bIsIncludeToBeDestroyed)
	{
		for (const std::shared_ptr<GameObject>& object : GetComponentView<Types ...>(bIsIncludeToBeDestroyed))
		{
			viewFunction(*object, object->template GetComponent<Types>() ...);
		}
	}

}