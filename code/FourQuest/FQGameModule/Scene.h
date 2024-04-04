#pragma once

#include "GameObjectIterator.h"
#include "GameObjectView.h"

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
		/// Game ���۽� ȣ��
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
		void ViewObjects(const std::function<void(const std::shared_ptr<GameObject>&)>& viewFunc
			, bool bIsIncludeToBeDestroyed = false);

	private:
		unsigned int mLastObjectID;
		std::vector<std::shared_ptr<GameObject>> mObjects;
	};

}
