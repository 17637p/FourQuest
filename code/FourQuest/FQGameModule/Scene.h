#pragma once

#include "GameObjectIterator.h"
#include "GameObjectView.h"
#include "ComponentIterator.h"
#include "ComponentView.h"
#include "GameObject.h"

namespace fq::game_module
{
	class GameObject;

	class Scene
	{
	public:
		/// <summary>
		/// ������
		/// </summary>
		Scene();

		/// <summary>
		/// �Ҹ���
		/// </summary>
		~Scene();

		/// <summary>
		/// 
		/// </summary>
		void Initialize();

		/// <summary>
		/// 
		/// </summary>
		void Start();

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dt"></param>
		void FixedUpdate(float dt);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dt"></param>
		void Update(float dt);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="dt"></param>
		void LateUpdate(float dt);

		/// <summary>
		/// Scene ����� ȣ��
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� ������ ������Ʈ ����
		/// </summary>
		void CleanUp();

		/// <summary>
		/// Scene�� ������ ������Ʈ ���� ��ȯ
		/// </summary>
		/// <returns>������Ʈ ����</returns>
		size_t GetObjectSize()const { return mObjects.size(); }

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
		unsigned int mLastObjectID;
		std::vector<std::shared_ptr<GameObject>> mObjects;
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