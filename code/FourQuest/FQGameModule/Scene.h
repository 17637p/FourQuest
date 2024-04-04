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
		/// 생성자
		/// </summary>
		Scene();

		/// <summary>
		/// 소멸자
		/// </summary>
		~Scene();

		/// <summary>
		/// 
		/// </summary>
		void Initialize();

		/// <summary>
		/// Game 시작시 호출
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
		/// Scene 종료시 호출
		/// </summary>
		void Finalize();

		/// <summary>
		/// Scene이 가지는 오브젝트 숫자 반환
		/// </summary>
		/// <returns>오브젝트 숫자</returns>
		size_t GetObjectSize()const { return mObjects.size(); }

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
		void ViewObjects(const std::function<void(const std::shared_ptr<GameObject>&)>& viewFunc
			, bool bIsIncludeToBeDestroyed = false);

	private:
		unsigned int mLastObjectID;
		std::vector<std::shared_ptr<GameObject>> mObjects;
	};

}
