#include "Physics.h"

namespace fq::physics
{
	Physics::Physics()
		: mDispatcher(nullptr)
		, mFoundation(nullptr)
		, mPhysics(nullptr)
		, mPvd(nullptr)
		, mPvdClient(nullptr)
	{

	}

	Physics::~Physics()
	{
		PX_RELEASE(mDispatcher);
		PX_RELEASE(mPhysics);

		if (mPvd)
		{
			physx::PxPvdTransport* transport = mPvd->getTransport();
			mPvd->release();
			mPvd = nullptr;
			PX_RELEASE(transport);
		}

		PX_RELEASE(mFoundation);
	}

	void Physics::Initialize()
	{
		// PhysX Foundation�� �����ϰ� �ʱ�ȭ�մϴ�.
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);

		// Foundation�� ���������� �����Ǿ����� Ȯ���մϴ�.
		if (!mFoundation)
		{
			throw("PxCreateFoundation failed!"); // Foundation ������ ������ ��� ���ܸ� throw�մϴ�.
		}

#ifdef _DEBUG
		// PhysX Visual Debugger (PVD)�� �����ϰ� �����մϴ�.
		mPvd = physx::PxCreatePvd(*mFoundation); // Foundation�� ����Ͽ� PVD�� �����մϴ�.
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10); // PVD�� ����� Ʈ������Ʈ�� �����մϴ�.
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL); // PVD�� Ʈ������Ʈ�� �����մϴ�.

		// ���� �ùķ��̼��� ��� ���� �������� �����մϴ�.
		mToleranceScale.length = 100; // ���� ��� ���� �������� �����մϴ�.
		mToleranceScale.speed = 981; // �ӵ� ��� ���� �������� �����մϴ�.

		// PhysX Physics�� �����ϰ� �ʱ�ȭ�մϴ�.
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd); // Physics�� �����մϴ�.

		// CPU ����ó�� �����ϰ� �����մϴ�.
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(3); // CPU ����ó�� �����մϴ�.
#else
		// ���� �ùķ��̼��� ��� ���� �������� �����մϴ�.
		mToleranceScale.length = 100; // ���� ��� ���� �������� �����մϴ�.
		mToleranceScale.speed = 981; // �ӵ� ��� ���� �������� �����մϴ�.

		// PhysX Physics�� �����ϰ� �ʱ�ȭ�մϴ�.
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, nullptr); // Physics�� �����մϴ�.

		// CPU ����ó�� �����ϰ� �����մϴ�.
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2); // CPU ����ó�� �����մϴ�.
#endif
	}

	void Physics::SettingPVDClient(physx::PxScene* scene)
	{
		mPvdClient = scene->getScenePvdClient();
		if (mPvdClient)
		{
			mPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			mPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			mPvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
	}
}
