#pragma once

#include <memory>
#include <map>
#include <set>
#include <PxPhysicsAPI.h>

#include "FQCommonPhysics.h"

namespace fq::physics
{
	class CharacterController;
	class PhysicsCollisionDataManager;

	class PhysicsCharactorControllerManager
	{
	public:
		PhysicsCharactorControllerManager();
		~PhysicsCharactorControllerManager();

		/// <summary>
		/// 캐릭터 컨트롤러 매니저 생성 및 초기화 함수
		/// </summary>
		bool initialize(physx::PxScene* scene, physx::PxPhysics* physics, std::shared_ptr<PhysicsCollisionDataManager> collisionDataManager);

		/// <summary>
		/// 캐릭터 컨트롤러 매니저가 소유하고 있는 캐릭터 컨트롤러 업데이트 함수
		/// </summary>
		bool Update(float deltaTime);

		/// <summary>
		/// 캐릭터 컨트롤러 유저 데이터 클리어용 업데이트
		/// </summary>
		bool FinalUpdate();

		/// <summary>
		/// 캐릭터 컨트롤러 생성 함수
		/// </summary>
		/// <param name="controllerInfo"> 캐릭터 컨트롤러 생성 데이터 </param>
		/// <param name="movementInfo"> 캐릭터 무브먼트 생성 데이터 </param>
		/// <returns></returns>
		bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo, int* collisionMatrix);

		/// <summary>
		/// 특정 캐릭터 컨트롤러의 입력 값 추가 함수
		/// </summary>
		/// <param name="id"> 캐릭터 컨트롤러 아이디 </param>
		/// <param name="input"> 입력한 이동 방향 (ex. {1.f, 0.f, 0.f}) </param>
		bool AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input);

		/// <summary>
		/// 캐릭터 컨트롤러 삭제 함수
		/// </summary>
		/// <param name="id">  </param>
		/// <returns></returns>
		bool RemoveController(const unsigned int& id);

		/// <summary>
		/// 캐릭터 컨트롤러 삭제 함수
		/// </summary>
		bool RemoveAllController();

		/// <summary>
		/// 충돌 매트릭스가 수정이 된다면 모든 리지드 바디들의 충돌 매트릭스를 업데이트합니다.
		/// </summary>
		/// <param name="collisionMatrix"></param>
		void UpdateCollisionMatrix(int* collisionMatrix);

		/// <summary>
		/// 캐릭터 컨트롤러와 캐릭터 무브먼트의 데이터를 Get Set합니다.
		/// </summary>
		void GetCharacterControllerData(const unsigned int& id, CharacterControllerGetSetData& data);
		void GetCharacterMovementData(const unsigned int& id, CharacterMovementGetSetData& data);
		void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData, int* collisionMatrix);
		void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData);

	private:
		std::weak_ptr<PhysicsCollisionDataManager> mCollisionDataManager;
		std::unordered_map<unsigned int, std::shared_ptr<CharacterController>>	mCCTmap;

		physx::PxPhysics*				mPhysics;
		physx::PxMaterial*				mMaterial;
		physx::PxControllerManager*		mCCTManager;
	};
}