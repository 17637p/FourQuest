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
	/// Component들을 저장하고 관리하는 컨테이너
	/// </summary>
	class GameObject : public std::enable_shared_from_this<fq::game_module::GameObject>
	{
		inline static unsigned int LastID = 0;

	public:
		using ComponentContainer = std::unordered_map<entt::id_type, std::unique_ptr<Component>>;

		/// <summary>
		/// 기본 생성자
		/// </summary>
		GameObject();
		
		/// <summary>
		/// 복사 생성자
		/// </summary>
		/// <param name="other"></param>
		GameObject(const GameObject& other);

		/// <summary>
		/// 복사 대입 연산자
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		GameObject& operator=(const GameObject& other);
		
		/// <summary>
		/// 소멸자 
		/// </summary>
		~GameObject();

		GameObject(const GameObject&& other) = delete;

		/// <summary>
		/// Gameobject가 프리팹에서 인스턴스화 후에 호출
		/// </summary>
		void OnAwake();

		/// <summary>
		/// Scene 시작시 호출
		/// </summary>
		void OnStart();
		
		/// <summary>
		/// 고정된 프레임으로 호출
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		void OnFixedUpdate(float dt);
		
		/// <summary>
		/// 매 프레임 호출
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void OnUpdate(float dt);

		/// <summary>
		///  Scene의 GameObject의 Update 이후에 호출
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void OnLateUpdate(float dt);
		
		/// <summary>
		/// GameObject 파괴시 호출
		/// </summary>
		void OnDestroy();

		/// <summary>
		/// 오브젝트가 삭제예정인지 확인
		/// </summary>
		/// <returns>삭제예정인지</returns>
		bool IsToBeDestroyed()const { return mbIsToBeDestroyed; }

		/// <summary>
		/// 게임오브젝트를 소유하는 현재 씬을 반환
		/// </summary>
		/// <returns>씬 반환</returns>
		Scene* GetScene()const { return mScene; }

		/// <summary>
		/// 게임오브젝트와 씬을 연결합니다
		/// </summary>
		/// <param name="scene">연결할 씬</param>
		void SetScene(Scene* scene) { mScene = scene; }

		/// <summary>
		/// 오브젝트의 이름을 반환합니다
		/// </summary>
		/// <returns>오브젝트 이름</returns>
		std::string GetName()const { return mName; }

		/// <summary>
		/// 오브젝트의 이름을 설정합니다.
		/// </summary>
		/// <param name="name">설정할 이름</param>
		void SetName(std::string name);

		/// <summary>
		/// 오브젝트의 고유ID 반환합니다
		/// </summary>
		/// <returns>ID 반환</returns>
		unsigned int GetID()const { return mID; }

		/// <summary>
		/// 계층구조의 부모를 반환합니다 
		/// </summary>
		/// <returns>없으면 nullptr, 그렇지 않으면 부모 포인터</returns>
		GameObject* GetParent();

		/// <summary>
		/// 계층구조의 자식들을 반환합니다
		/// * std::vector 할당으로 효율은 좋지않습니다
		/// </summary>
		/// <returns>자식 오브젝트 배열</returns>
		std::vector<GameObject*> GetChildren();

		/// <summary>
		/// T타입 컴포넌트를 반환합니다
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <returns>가지고있는 컴포넌트</returns>
		template <typename T>
		T* GetComponent();

		/// <summary>
		/// 컴포넌트들을 담은 컨테이너를 반환합니다
		/// </summary>
		/// <returns>컴포넌트 컨테이너</returns>
		const ComponentContainer& GetComponentContainer()const { return mComponents; }

		/// <summary>
		/// T타입 컴포넌트를 소유하는지 확인합니다
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <returns>가지면 true 반환</returns>
		template <typename T>
		bool HasComponent()const;

		/// <summary>
		/// T... 에 해당하는 컴포넌트들을 모두 소유하는지
		/// </summary>
		/// <typeparam name="T1">컴포넌트 타입</typeparam>
		/// <typeparam name="T2">컴포넌트 타입</typeparam>
		/// <typeparam name="...Types">컴포넌트 타입...</typeparam>
		/// <returns>모두 가지면 true 아니면 false</returns>
		template <typename T1, typename T2, typename... Types>
		bool HasComponent()const;

		/// <summary>
		/// 컴포넌트 추가합니다
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <typeparam name="...Args">생성자 오버로딩 가변인자</typeparam>
		/// <param name="...args">생성자 인자</param>
		/// <returns>생성한 컴포넌트 반환</returns>
		template <typename T, typename... Args>
		T& AddComponent(Args&&... args);

		/// <summary>
		/// any 컴포넌트의 복사본은 추가합니다
		/// </summary>
		/// <param name="any">Component</param>
		void AddComponent(const entt::meta_any& any);

	private:
		unsigned int mID;
		std::string mName;
		Tag mTag;
		ComponentContainer mComponents;
		Scene* mScene;
		bool mbIsToBeDestroyed;

		friend class Scene;
		FQ_REGISTRATION_FRIEND
	};

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
			&& "가지고있는 컴포넌트입니다.");

		mComponents.insert({ id, std::make_unique<T>(std::forward<Args>(args)...) });

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


