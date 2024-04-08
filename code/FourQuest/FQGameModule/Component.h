#pragma once

namespace fq::game_module
{
	class GameObject;

	/// <summary>
	/// GameOnject에 귀속되어 실질적인 
	/// 게임 로직을 수행
	/// </summary>
	class Component
	{
	public:
		Component();
		Component(const Component& other) = default;
		Component& operator =(const Component& other) = default;
		Component(Component&& other) = default;

		virtual ~Component();

		/// <summary>
		/// GameObject가 프리팹에서 인스턴스화 후에 호출
		/// </summary>
		virtual void OnAwake() {};

		/// <summary>
		/// Scene 시작시 호출
		/// </summary>
		virtual void OnStart() {};
		
		/// <summary>
		/// 고정된 프레임으로 호출
		/// </summary>
		/// <param name="dt">FixedDeltaTime</param>
		virtual void OnFixedUpdate(float dt) {};
		
		/// <summary>
		/// 매 프레임 호출
		/// 컴포넌트의 기본적인 로직을 수행
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnUpdate(float dt) {};
		
		/// <summary>
		/// Scene의 GameObject의 Update 이후에 호출
		/// </summary>
		/// <param name="dt">DeltaTime</param>
		virtual void OnLateUpdate(float dt) {};

		/// <summary>
		/// GameObject 파괴시 호출
		/// </summary>
		virtual void OnDestroy() {};

		/// <summary>
		/// 컴포넌트를 소유하는 게임오브젝트 반환
		/// </summary>
		/// <returns>게임 오브젝트</returns>
		GameObject* GetGameObject()const { return mGameObject; }

	private:
		GameObject* mGameObject;
	};


}