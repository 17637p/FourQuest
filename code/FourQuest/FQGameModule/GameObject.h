#pragma once


namespace fq::game_module
{
	class Scene;
	class Component;

	class GameObject
	{
	public:
		GameObject();

		GameObject(const GameObject& other);

		GameObject& operator=(const GameObject& other);

		~GameObject();

		void Initialize();

		void Start();

		void Update(float dt);

		void Finalize();

		/// <summary>
		/// ������Ʈ�� ������������ Ȯ��
		/// </summary>
		/// <returns>������������</returns>
		bool IsToBeDestroyed()const { return mbIsToBeDestroyed; }

		/// <summary>
		/// ������Ʈ�� ����ID ��ȯ
		/// </summary>
		/// <returns>ID ��ȯ</returns>
		unsigned int GetID()const { return mID; }

		/// <summary>
		/// TŸ�� ������Ʈ�� ��ȯ
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <returns>�������ִ� ������Ʈ</returns>
		template <typename T>
		T& GetComponent();

		/// <summary>
		/// TŸ�� ������Ʈ�� �����ϴ���
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <returns>������ true ��ȯ</returns>
		template <typename T>
		bool HasComponent()const;

		template <typename T1, typename T2, typename... Types>
		bool HasComponent()const;

		/// <summary>
		/// ������Ʈ �߰�
		/// </summary>
		/// <typeparam name="T">������Ʈ Ÿ��</typeparam>
		/// <typeparam name="...Args">������ �����ε� ��������</typeparam>
		/// <param name="...args">������ ����</param>
		/// <returns>������ ������Ʈ ��ȯ</returns>
		template <typename T, typename... Args>
		T& AddComponent(Args&&... args);

	private:
		unsigned int mID;
		std::string mName;
		Tag mTag;
		std::unordered_map<entt::id_type, std::unique_ptr<Component>> mComponents;
		Scene* mScene;
		bool mbIsToBeDestroyed;

		FQ_REGISTRATION_FRIEND
	};

	template <typename T>
	bool fq::game_module::GameObject::HasComponent() const
	{
		entt::id_type id = entt::type_id<T>().index();

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
		entt::id_type id = entt::type_id<T>().index();

		assert(mComponents.find(id) == mComponents.end() 
			&& "�������ִ� ������Ʈ�Դϴ�.");

		mComponents.insert({ id, std::make_unique<T>(std::forward(args)...) });

		T* component = static_cast<T*>(mComponents[id].get());
		return *component;
	}

	template <typename T>
	T& fq::game_module::GameObject::GetComponent()
	{
		entt::id_type id = entt::type_id<T>().index();

		auto iter = mComponents.find(id);
		assert(iter != mComponents.end());

		T* component = static_cast<T*>(iter->second.get());
		return *component;
	}

}