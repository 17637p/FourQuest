#pragma once

#include "GameObject.h"

namespace fq::game_module
{
	class Scene;

	/// <summary>
	/// GameOnject에 귀속되어 실질적인 
	/// 게임 로직을 수행
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
		/// 핸들을 반환합니다 
		/// </summary>
		virtual entt::meta_handle GetHandle() abstract;

		/// <summary>
		/// 복사본을 Component*로 반환합니다
		/// </summary>
		/// <param name="clone"></param>
		/// <returns></returns>
		virtual Component* Clone(Component* clone = nullptr)const abstract;

		/// <summary>
		/// Scene 시작시 호출
		/// </summary>
		virtual void OnStart() {}
		
		/// <summary>
		/// 고정된 프레임으로 호출
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		virtual void OnFixedUpdate(float dt) {}
		
		/// <summary>
		/// 매 프레임 호출
		/// 컴포넌트의 기본적인 로직을 수행
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnUpdate(float dt) {}
		
		/// <summary>
		/// Scene의 GameObject의 Update 이후에 호출합니다
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnLateUpdate(float dt) {}

		/// <summary>
		/// GameObject 파괴시 호출합니다
		/// </summary>
		virtual void OnDestroy() {}

		virtual void OnCollisionEnter(const Collision& collision) {}
		virtual void OnCollisionExit(const Collision& collision) {}
		virtual void OnCollisionStay(const Collision& collision) {}
		virtual void OnTriggerEnter(const Collision& collision) {}
		virtual void OnTriggerStay(const Collision& collision) {}
		virtual void OnTriggerExit(const Collision& collision) {}

		/// <summary>
		/// 컴포넌트를 소유하는 게임오브젝트 반환합니다
		/// </summary>
		/// <returns>게임 오브젝트</returns>
		GameObject* GetGameObject()const { return mGameObject; }

		/// <summary>
		/// Gameobject를 설정합니다
		/// </summary>
		/// <param name="object">게임오브젝트</param>
		void SetGameObject(GameObject* object) { mGameObject = object; }

		/// <summary>
		/// 게임오브젝트와 연결된 씬을 반환합니다.
		/// </summary>
		/// <returns>Scene Pointer</returns>
		Scene* GetScene()const;

		/// <summary>
		/// T타입의 컴포넌트를 반환합니다
		/// </summary>
		/// <typeparam name="T">컴포넌트 타입</typeparam>
		/// <returns>없으면 nullptr, 그렇지않으면 T타입 컴포넌트</returns>
		template <typename T>
		T* GetComponent();

		/// <summary>
		/// 삭제 예정인 컴포넌트인지 확인합니다
		/// </summary>
		/// <returns>삭제예정이면 true, 그렇지않으면 false</returns>
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