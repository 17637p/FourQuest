#pragma once
#include "IFQPhysics.h"
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING

#include <physx\PxPhysicsAPI.h>
#include <memory>
#include <set>

namespace fq::physics
{
	class Physics;
	class PhysicsRigidBodyManager;
	class PhysicsSimulationEventCallback;
	class PhysicsResourceManager;
	class PhysicsCharactorControllerManager;
	class PhysicsCharacterPhysicsManager;

	class FQPhysics : public IFQPhysics
	{
	public:
		FQPhysics();
		virtual ~FQPhysics();

		/// <summary>
		/// ���� ������ �����Ͽ� ���� ������ ����ϴ�.
		/// </summary>
		virtual bool Initialize(PhysicsEngineInfo& info) override;

		/// <summary>
		/// ���� ������ ������Ʈ�� ���� ������ �ٵ� �߰��ϰ� �ùķ��̼��� �����մϴ�.
		/// �ùķ��̼��� �������� �ÿ� �浹 �̺�Ʈ�� �ݹ� �Լ��� ���� ����˴ϴ�.
		/// </summary>
		virtual bool Update(float deltaTime) override;

		/// <summary>
		/// ����� ������ �� ���� ���� ������ Ŭ����� ������Ʈ
		/// </summary>
		virtual bool FinalUpdate() override;

		/// <summary>
		/// �ݹ� �Լ� ���
		/// </summary>
		/// <param name="func"> �ݸ��� ������ ����ü </param>
		/// <param name="eventType"> �ݸ��� �̺�Ʈ Ÿ�� </param>
		virtual void SetCallBackFunction(std::function<void(fq::physics::CollisionData, fq::physics::ECollisionEventType)> func) override;

		/// <summary>
		/// ���� ������ ������ ������ ���� �޽��ϴ�. ( �߷�, �浹 ��Ʈ���� )
		/// </summary>
		/// <param name="info"> ���� ���� ���� </param>
		virtual void SetPhysicsInfo(PhysicsEngineInfo& info) override;

#pragma region RigidBodyManager
		/// <summary>
		/// ���� ������ �߰��� ����ƽ �ٵ� �� ���̳��� �ٵ� �����մϴ�.
		/// </summary>
		virtual bool CreateStaticBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateStaticBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const BoxColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const SphereColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const CapsuleColliderInfo& info, const EColliderType& colliderType) override;
		virtual bool CreateDynamicBody(const ConvexMeshColliderInfo& info, const EColliderType& colliderType) override;

		/// <summary>
		/// ���̵� ������ �ش� ���̵��� ������ �ٵ� ��ȯ
		/// </summary>
		virtual RigidBodyGetSetData GetRigidBodyData(const unsigned int& id) override;

		/// <summary>
		/// ���̵� ������ �ش� ���̵��� ������ �ٵ𿡰� ������ Ʈ���������� �̵� ( �����̵� )
		/// </summary>
		virtual bool SetRigidBodyData(const unsigned int& id, const RigidBodyGetSetData& rigidBodyData) override;

		/// <summary>
		/// ���̵� ���� ���� ������ �ٵ� �����մϴ�.
		/// </summary>
		virtual bool RemoveRigidBody(const unsigned int& id) override;

		/// <summary>
		/// ���� ������ �ִ� ��� ������ �ٵ���� �����մϴ�.
		/// </summary>
		virtual bool RemoveAllRigidBody() override;

		/// <summary>
		/// �������� ����� ������ 
		/// </summary>
		const std::unordered_map<unsigned int, PolygonMesh>& GetDebugPolygon() override;
#pragma endregion

#pragma region CharacterControllerManager
		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		/// <param name="controllerInfo"> ĳ���� ��Ʈ�ѷ� ���� ������ </param>
		/// <param name="movementInfo"> ĳ���� �����Ʈ ���� ������ </param>
		virtual bool CreateCCT(const CharacterControllerInfo& controllerInfo, const CharacterMovementInfo& movementInfo) override;

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ� ���� �Լ�
		/// </summary>
		/// <param name="id"> ������ ĳ���� ��Ʈ�ѷ��� ���̵� </param>
		virtual bool RemoveController(const unsigned int& id) override;

		/// <summary>
		/// Ư�� ĳ���� ��Ʈ�ѷ��� �Է� �� �߰� �Լ�
		/// </summary>
		/// <param name="id"> ĳ���� ��Ʈ�ѷ� ���̵� </param>
		/// <param name="input"> �Է��� �̵� ���� (ex. {1.f, 0.f, 0.f}) </param>
		virtual bool AddInputMove(const unsigned int& id, const DirectX::SimpleMath::Vector3& input) override;

		/// <summary>
		/// ĳ���� ��Ʈ�ѷ��� ĳ���� �����Ʈ�� �����͸� Get Set�մϴ�.
		/// </summary>
		virtual CharacterControllerGetSetData GetCharacterControllerData(const unsigned int& id) override;
		virtual CharacterMovementGetSetData GetCharacterMovementData(const unsigned int& id) override;
		virtual void SetCharacterControllerData(const unsigned int& id, const CharacterControllerGetSetData& controllerData) override;
		virtual void SetCharacterMovementData(const unsigned int& id, const CharacterMovementGetSetData& movementData) override;
#pragma endregion

#pragma region CharacterPhysicsManager
		/// <summary>
		/// ĳ���� ������ (����) �߰�
		/// </summary>
		virtual bool CreateCharacterphysics(const CharacterPhysicsInfo& info) override;

		/// <summary>
		/// ������ �ִ� ���� ��, ��ũ �� ����Ʈ �߰�
		/// </summary>
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const DirectX::SimpleMath::Vector3& extent) override;
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& radius) override;
		virtual bool AddArticulationLink(unsigned int id, const CharacterLinkInfo& info, const float& halfHeight, const float& radius) override;

		/// <summary>
		/// ���� ������ �߰��Ͽ� CharacterPhysics�� �ùķ��̼��� ĳ���� ������
		/// </summary>
		virtual bool SimulationCharacter(unsigned int id) override;
#pragma endregion

		virtual bool HasConvexMeshResource(const unsigned int& hash) override;

		/// <summary>
		/// spdlog�� �����մϴ�
		/// </summary>
		virtual FQ_PHYSICS std::shared_ptr<spdlog::logger> SetUpLogger(std::vector<spdlog::sink_ptr> sinks) override;

	private:
		// ��
		physx::PxScene* mScene;
		std::shared_ptr<Physics> mPhysics;
		std::shared_ptr<PhysicsResourceManager> mResourceManager;
		std::shared_ptr<PhysicsRigidBodyManager> mRigidBodyManager;
		std::shared_ptr<PhysicsCharactorControllerManager> mCCTManager;
		std::shared_ptr<PhysicsCharacterPhysicsManager> mCharacterPhysicsManager;

		// �浹 �̺�Ʈ Ŭ����
		std::shared_ptr<PhysicsSimulationEventCallback> mMyEventCallback;

		// �浹 ��Ʈ����
		int mCollisionMatrix[16];
	};
}

