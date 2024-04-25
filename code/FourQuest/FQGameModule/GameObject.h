#pragma once

#include <unordered_map>
#include <memory>

#include "../FQReflect/FQReflect.h"

#include "GameModuleEnum.h"

namespace fq::game_module
{
	class Scene;
	class Component;

	/// <summary>
	/// Component���� �����ϰ� �����ϴ� �����̳�
	/// </summary>
	class GameObject : 
		public std::enable_shared_from_this<GameObject>,
		public fq::reflect::IHandle
	{
		inline static unsigned int LastID = 0;

	public:
		using ComponentContainer = std::unordered_map<entt::id_type, std::shared_ptr<Component>>;

		/// <summary>
		/// �⺻ ������
		/// </summary>
		GameObject();

		/// <summary>
		/// ���� ������
		/// </summary>
		GameObject(const GameObject& other);

		/// <summary>
		/// ���� ���� ������
		/// </summary>
		GameObject& operator=(const GameObject& other);

		/// <summary>
		/// �Ҹ��� 
		/// </summary>
		~GameObject();

		GameObject(const GameObject&& other) = delete;
		
		/// <summary>
		/// Scene ���۽� ȣ��
		/// </summary>
		void OnStart();

		/// <summary>
		/// ������ ���������� ȣ��
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		void OnFixedUpdate(float dt);

		/// <summary>
		/// �� ������ ȣ��
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void OnUpdate(float dt);

		/// <summary>
		///  Scene�� GameObject�� Update ���Ŀ� ȣ��
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void OnLateUpdate(float dt);

		/// <summary>
		/// GameObject �ı��� ȣ��
		/// </summary>
		void OnDestroy();

		/// <summary>
		/// ������Ʈ�� ������������ Ȯ��
		/// </summary>
		/// <returns>������������</returns>
		bool IsDestroyed()const { return mbIsDestroyed; }

		/// <summary>
		/// ���ӿ�����Ʈ�� �����ϴ� ���� ���� ��ȯ
		/// </summary>
		/// <returns>�� ��ȯ</returns>
		Scene* GetScene()const { return mScene; }

		/// <summary>
		/// ���ӿ�����Ʈ�� ���� �����մϴ�
		/// </summary>
		/// <param name="scene">������ ��</param>
		void SetScene(Scene* scene) { mScene = scene; }

		/// <summary>
		/// ������Ʈ�� �̸��� ��ȯ�մϴ�
		/// </summary>
		/// <returns>������Ʈ �̸�</returns>
		std::string GetName()const { return mName; }

		/// <summary>
		/// ������Ʈ�� �̸��� �����մϴ�.
		/// </summary>
		/// <param name="name">������ �̸�</param>
		void SetName(std::string name);

		/// <summary>
		/// ������Ʈ�� ����ID ��ȯ�մϴ�
		/// </summary>
		/// <returns>ID ��ȯ</returns>
		unsigned int GetID()const { return mID; }

		/// <summary>
		/// ������Ʈ�� Tag�� ��ȯ�մϴ�
		/// </summary>
		/// <returns>������Ʈ Tag</returns>
		ETag GetTag()const { return mTag; }

		/// <summary>
		/// Tag�� �����մϴ�
		/// </summary>
		/// <param name="tag">�±�</param>
		void SetTag(ETag tag) { mTag = tag; }

		/// <summary>
		/// ���������� �θ� ��ȯ�մϴ� 
		/// </summary>
		/// <returns>������ nullptr, �׷��� ������ �θ� ������</returns>
		GameObject* GetParent();

		/// <summary>
		/// �θ� ������ �ִ��� Ȯ���մϴ�
		/// </summary>
		/// <returns>�θ������� true, �׷��������� false</returns>
		bool HasParent();

		/// <summary>
		/// ���������� �ڽĵ��� ��ȯ�մϴ�
		/// * std::vector �Ҵ����� ȿ���� �����ʽ��ϴ�
		/// </summary>
		/// <returns>�ڽ� ������Ʈ �迭</returns>
		std::vector<GameObject*> GetChildren();

		/// <summary>
		/// ���ӿ�����Ʈ�� �����ϴ� ��� ������Ʈ�� �����մϴ�
		/// </summary>
		void RemoveAllComponent();

		/// <summary>
		/// id_type�� �ش��ϴ� ������Ʈ�� �����մϴ�
		/// </summary>
		/// <param name="id">id_type</param>
		void RemoveComponent(entt::id_type id);

		/// <summary>
		/// TŸ�� ������Ʈ�� ��ȯ�մϴ�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <returns>�������ִ� ������Ʈ</returns>
		template <typename T>
		T* GetComponent();

		/// <summary>
		/// id_type�� �ش��ϴ� ������Ʈ�� ��ȯ�մϴ�
		/// </summary>
		/// <param name="id">���̵� Ÿ��</param>
		/// <returns>������ nullptr, �׷��������� ��ȿ�� ������Ʈ</returns>
		Component* GetComponent(entt::id_type id);

		/// <summary>
		/// ������Ʈ���� ���� �����̳ʸ� ��ȯ�մϴ�
		/// </summary>
		/// <returns>������Ʈ �����̳�</returns>
		const ComponentContainer& GetComponentContainer()const { return mComponents; }

		/// <summary>
		/// TŸ�� ������Ʈ�� �����ϴ��� Ȯ���մϴ�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <returns>������ true ��ȯ</returns>
		template <typename T>
		bool HasComponent()const;

		/// <summary>
		/// T... �� �ش��ϴ� ������Ʈ���� ��� �����ϴ���
		/// </summary>
		/// <typeparam name="T1">������Ʈ Ÿ��</typeparam>
		/// <typeparam name="T2">������Ʈ Ÿ��</typeparam>
		/// <typeparam name="...Types">������Ʈ Ÿ��...</typeparam>
		/// <returns>��� ������ true �ƴϸ� false</returns>
		template <typename T1, typename T2, typename... Types>
		bool HasComponent()const;

		/// <summary>
		/// ������Ʈ �߰��մϴ�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <typeparam name="...Args">������ �����ε� ��������</typeparam>
		/// <param name="...args">������ ����</param>
		/// <returns>������ ������Ʈ ��ȯ</returns>
		template <typename T, typename... Args>
		T& AddComponent(Args&&... args);

		/// <summary>
		/// any ������Ʈ�� ���纻�� �߰��մϴ�
		/// </summary>
		/// <param name="any">Component</param>
		void AddComponent(entt::meta_any any);

		/// <summary>
		/// id Ÿ�Կ� �ش��ϴ� ������Ʈ�� 
		/// </summary>
		/// <param name="id"></param>
		/// <param name="component"></param>
		void AddComponent(entt::id_type id, std::shared_ptr<Component> component);

		/// <summary>
		/// T Ÿ�Կ� �ش��ϴ� ������Ʈ�� �ı��մϴ�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		template <typename T>
		void RemoveComponent();

		/// <summary>
		/// ���� ������ ������Ʈ�� �����մϴ�
		/// </summary>
		void CleanUpComponent();

	private:
		entt::meta_handle GetHandle()override;

	private:
		unsigned int mID;
		std::string mName;
		ETag mTag;
		ComponentContainer mComponents;
		Scene* mScene;
		bool mbIsDestroyed;

		friend class Scene;
	};

	template <typename T>
	void fq::game_module::GameObject::RemoveComponent()
	{
		entt::id_type id = entt::resolve<T>().id();

		RemoveComponent(id);
	}

	template <typename T>
	bool fq::game_module::GameObject::HasComponent() const
	{
		entt::id_type id = entt::resolve<T>().id();

		auto iter = mComponents.find(id);

		return iter != mComponents.end();
	}

	template<typename T1, typename T2, typename ...Types>
	inline bool GameObject::HasComponent() const
	{
		return HasComponent<T1>() && HasComponent<T2, Types...>();
	}

	template<typename T, typename ...Args>
	inline T& GameObject::AddComponent(Args && ...args)
	{
		entt::id_type id = entt::resolve<T>().id();

		assert(mComponents.find(id) == mComponents.end()
			&& "�������ִ� ������Ʈ�Դϴ�.");

		mComponents.insert({ id, std::make_shared<T>(std::forward<Args>(args)...) });

		T* component = static_cast<T*>(mComponents[id].get());

		component->SetGameObject(this);

		return *component;
	}

	template <typename T>
	T* fq::game_module::GameObject::GetComponent()
	{
		entt::id_type id = entt::resolve<T>().id();

		auto iter = mComponents.find(id);

		if (iter == mComponents.end())
		{
			return nullptr;
		}

		T* component = static_cast<T*>(iter->second.get());
		return component;
	}

}


