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
		/// 오브젝트가 삭제예정인지 확인
		/// </summary>
		/// <returns>삭제예정인지</returns>
		bool IsToBeDestroyed()const { return mbIsToBeDestroyed; }

		/// <summary>
		/// 오브젝트의 고유ID 반환
		/// </summary>
		/// <returns>ID 반환</returns>
		unsigned int GetID()const { return mID; }

		/// <summary>
		/// T타입 컴포넌트를 반환
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <returns>가지고있는 컴포넌트</returns>
		template <typename T>
		T& GetComponent();

		/// <summary>
		/// T타입 컴포넌트를 소유하는지
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <returns>가지면 true 반환</returns>
		template <typename T>
		bool HasComponent()const;

		template <typename T1, typename T2, typename... Types>
		bool HasComponent()const;

		/// <summary>
		/// 컴포넌트 추가
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <typeparam name="...Args">생성자 오버로딩 가변인자</typeparam>
		/// <param name="...args">생성자 인자</param>
		/// <returns>생성한 컴포넌트 반환</returns>
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
			&& "가지고있는 컴포넌트입니다.");

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