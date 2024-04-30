#pragma once

#include "GameObject.h"

namespace fq::game_module
{
	class Scene;

	/// <summary>
	/// GameOnject�� �ͼӵǾ� �������� 
	/// ���� ������ ����
	/// </summary>
	class Component : 
		public std::enable_shared_from_this<Component>,
		public fq::reflect::IHandle
	{
	public:
		Component();
		Component(const Component& other) = default;
		Component& operator =(const Component& other) = default;
		Component(Component&& other) = default;

		virtual ~Component();

		/// <summary>
		/// �ڵ��� ��ȯ�մϴ� 
		/// </summary>
		virtual entt::meta_handle GetHandle() abstract;

		/// <summary>
		/// ���纻�� Component*�� ��ȯ�մϴ�
		/// </summary>
		/// <param name="clone"></param>
		/// <returns></returns>
		virtual Component* Clone(Component* clone = nullptr)const abstract;

		/// <summary>
		/// Scene ���۽� ȣ��
		/// </summary>
		virtual void OnStart() {}
		
		/// <summary>
		/// ������ ���������� ȣ��
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		virtual void OnFixedUpdate(float dt) {}
		
		/// <summary>
		/// �� ������ ȣ��
		/// ������Ʈ�� �⺻���� ������ ����
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnUpdate(float dt) {}
		
		/// <summary>
		/// Scene�� GameObject�� Update ���Ŀ� ȣ���մϴ�
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnLateUpdate(float dt) {}

		/// <summary>
		/// GameObject �ı��� ȣ���մϴ�
		/// </summary>
		virtual void OnDestroy() {}

		virtual void OnCollisionEnter(const Collision& collision) {}
		virtual void OnCollisionExit(const Collision& collision) {}
		virtual void OnCollisionStay(const Collision& collision) {}
		virtual void OnTriggerEnter(const Collision& collision) {}
		virtual void OnTriggerStay(const Collision& collision) {}
		virtual void OnTriggerExit(const Collision& collision) {}

		/// <summary>
		/// ������Ʈ�� �����ϴ� ���ӿ�����Ʈ ��ȯ�մϴ�
		/// </summary>
		/// <returns>���� ������Ʈ</returns>
		GameObject* GetGameObject()const { return mGameObject; }

		/// <summary>
		/// Gameobject�� �����մϴ�
		/// </summary>
		/// <param name="object">���ӿ�����Ʈ</param>
		void SetGameObject(GameObject* object) { mGameObject = object; }

		/// <summary>
		/// ���ӿ�����Ʈ�� ����� ���� ��ȯ�մϴ�.
		/// </summary>
		/// <returns>Scene Pointer</returns>
		Scene* GetScene()const;

		/// <summary>
		/// TŸ���� ������Ʈ�� ��ȯ�մϴ�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <returns>������ nullptr, �׷��������� TŸ�� ������Ʈ</returns>
		template <typename T>
		T* GetComponent();

		/// <summary>
		/// ���� ������ ������Ʈ���� Ȯ���մϴ�
		/// </summary>
		/// <returns>���������̸� true, �׷��������� false</returns>
		bool IsTobeRemoved()const { return mbIsToBeRemoved; }

	private:
		GameObject* mGameObject;
		bool mbIsToBeRemoved;

		friend class GameObject;
	};

	template <typename T>
	T* fq::game_module::Component::GetComponent()
	{
		return mGameObject-> template GetComponent<T>();
	}

}