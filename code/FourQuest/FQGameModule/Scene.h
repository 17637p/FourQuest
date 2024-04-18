#pragma once

#include "GameObjectIterator.h"
#include "GameObjectView.h"
#include "ComponentIterator.h"
#include "ComponentView.h"
#include "GameObject.h"


namespace fq::game_module
{
	class GameObject;
	class EventManager;
	class InputManager;

	class Scene
	{
	public:
		Scene();
		~Scene();

		/// <summary>
		/// Scene의 데이터를 로드한다
		/// </summary>
		void Initialize(std::string sceneName, EventManager* eventMgr, InputManager* inputMgr);

		/// <summary>
		/// 삭제 예정인 오브젝트 제거합니다
		/// </summary>
		void CleanUp();

		/// <summary>
		/// Scene 게임오브젝트를 추가합니다
		/// 오브젝트의 자식계층까지 모두 추가합니다.
		/// </summary>
		/// <param name="object">오브젝트</param>
		void AddGameObject(std::shared_ptr<GameObject> object);

		/// <summary>
		/// Scene 내부의 게임오브젝트를 삭제합니다
		/// </summary>
		/// <param name="object"></param>
		void DestroyGameObject(GameObject* object);

		/// <summary>
		/// Scene내부의 모든 게임 오브젝트를 삭제합니다
		/// </summary>
		void DestroyAll();

		/// <summary>
		/// Scene이 가지는 오브젝트 숫자 반환
		/// </summary>
		/// <returns>오브젝트 숫자</returns>
		size_t GetObjectSize()const { return mObjects.size(); }

		/// <summary>
		/// 현재 씬의 이름을 반환
		/// </summary>
		/// <returns>씬의 이름</returns>
		std::string GetSceneName()const { return mSceneName; }

		/// <summary>
		/// 인덱스에 해당하는 오브젝트 반환
		/// !! GameObject ID가 아닙니다 !!
		/// </summary>
		/// <param name="index">인덱스</param>
		/// <returns>index를 초과한경우 nulllptr 이고 아니면 정상적인 게임 오브젝트</returns>
		std::shared_ptr<GameObject> GetObjectByIndex(size_t index);

		/// <summary>
		/// 오브젝트 ID에 해당하는 오브젝트 반환
		/// </summary>
		/// <param name="id">오브젝트의 고유 ID</param>
		/// <returns>찾지못한경우 nullptr 반환하고 아니면 id를 소유하는 객체반환</returns>
		std::shared_ptr<GameObject> GetObjectByID(unsigned int id);

		/// <summary>
		/// Scene의 게임오브젝트를 순회하는 view 객체를 반환 
		/// </summary>
		/// <param name="bIsIncludeToBeDestroyed">삭제예정인 오브젝트를 포함 여부</param>
		/// <returns>게임오브젝트 view 객체</returns>
		internal::GameObjectView GetObjectView(bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// 게임오브젝트 순회하면서 viewFunc을 호출한다.
		/// </summary>
		/// <param name="viewFunc">호출할 함수</param>
		/// <param name="bIsIncludeToBeDestroyed">삭제 예정인 오브젝트 포함 여부</param>
		void ViewObjects(std::function<void(GameObject&)> viewFunction
			, bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// 가변인자에 해당하는 컴포넌트 타입들을 가진 
		/// 오브젝트들을 쿼리하여 순회하는 View객체 반환
		/// </summary>
		/// <typeparam name="...Types">쿼리할 컴포넌트 타입들</typeparam>
		/// <param name="bIsIncludeToBeDestroyed">삭제 예정 오브젝트 포함 여부</param>
		/// <returns>순회가능한 View객체 반환</returns>
		template <typename... Types>
		internal::ComponentView<Types...> GetComponentView(bool bIsIncludeToBeDestroyed = false);

		/// <summary>
		/// 가변인자에 해당하는 컴포넌트 타입들을 가진 
		/// 오브젝트들을 쿼리하여 viewFunction을 호출
		/// </summary>
		/// <typeparam name="...Types">쿼리할 컴포넌트 타입</typeparam>
		/// <param name="viewFunction">쿼리할 객체에서 실행할 함수</param>
		/// <param name="bIsIncludeToBeDestroyed">삭제 예정 오브젝트 포함 여부</param>
		template <typename... Types>
		void ViewComponents(typename std::common_type_t<std::function<void(GameObject&, Types& ...)>> viewFunction,
			bool bIsIncludeToBeDestroyed = false);

	private:
		void destroyChild(GameObject* object);

	private:
		std::string mSceneName;
		std::vector<std::shared_ptr<GameObject>> mObjects;

		InputManager* mInputManager;
		EventManager* mEventManager;

		friend class SceneManager;
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