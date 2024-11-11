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
        /// ĳ���� ��Ʈ�ѷ� �ʱ�ȭ �Լ�
        /// </summary>
        /// <param name="info"> ĳ���� ��Ʈ�ѷ� �ʱ�ȭ ������ </param>
        /// <param name="movementInfo"> ĳ���� �����Ʈ �ʱ�ȭ ������ </param>
        /// <param name="CCTManager"> PxController�� ������ PxControllerManager </param>
        /// <param name="material"> PxController�� �����ϱ� ���� material </param>
        /// <returns></returns>
        virtual bool Initialize(const CharacterControllerInfo& info
            , const CharacterMovementInfo& movementInfo
            , physx::PxControllerManager* CCTManager
            , physx::PxMaterial* material
            , std::shared_ptr<CollisionData> collisionData
            , int* collisionMatrix) override;
        
        /// <summary>
        /// �Է� ���� �Է� ���� ������ �̵��ϰų� �����ϴ� �Լ�
        /// </summary>
        /// <param name="deltaTime"> ��� �ð� </param>
        /// <returns></returns>
        virtual bool Update(float deltaTime) override;

    private:
    };

}