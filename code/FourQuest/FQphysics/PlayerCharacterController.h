#pragma once
#include "CharacterController.h"

namespace fq::physics
{
    class PhysicsCCTHitCallback;

    class PlayerCharacterController :
        public CharacterController
    {
    public:
        PlayerCharacterController();
        virtual ~PlayerCharacterController();

        /// <summary>
        /// 캐릭터 컨트롤러 초기화 함수
        /// </summary>
        /// <param name="info"> 캐릭터 컨트롤러 초기화 데이터 </param>
        /// <param name="movementInfo"> 캐릭터 무브먼트 초기화 데이터 </param>
        /// <param name="CCTManager"> PxController를 생성할 PxControllerManager </param>
        /// <param name="material"> PxController를 생성하기 위한 material </param>
        /// <returns></returns>
        virtual bool Initialize(const CharacterControllerInfo& info
            , const CharacterMovementInfo& movementInfo
            , physx::PxControllerManager* CCTManager
            , physx::PxMaterial* material
            , std::shared_ptr<CollisionData> collisionData
            , int* collisionMatrix) override;
        
        /// <summary>
        /// 입력 받은 입력 값을 가지고 이동하거나 점프하는 함수
        /// </summary>
        /// <param name="deltaTime"> 경과 시간 </param>
        /// <returns></returns>
        virtual bool Update(float deltaTime) override;

    private:
    };

}