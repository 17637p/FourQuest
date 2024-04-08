#pragma once

#include <unordered_map>

#include "../FQReflect/FQReflect.h"

#include "GameModuleEnum.h"

namespace fq::game_module
{
	class Scene;
	class Component;

	/// <summary>
	/// Component들을 저장하고 관리하는 컨테이너
	/// </summary>
	class GameObject
	{
	public:
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
		void Start();
		
		/// <summary>
		/// 고정된 프레임으로 호출
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		void FixedUpdate(float dt);
		
		/// <summary>
		/// 매 프레임 호출
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void Update(float dt);

		/// <summary>
		///  Scene의 GameObject의 Update 이후에 호출
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		void LateUpdate(float dt);
		
		/// <summary>
		/// GameObject 파괴시 호출
		/// </summary>
		void Destroy();

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