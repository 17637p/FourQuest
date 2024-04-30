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
		// PhysX Foundation을 생성하고 초기화합니다.
		mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);

		// Foundation이 성공적으로 생성되었는지 확인합니다.
		if (!mFoundation)
		{
			throw("PxCreateFoundation failed!"); // Foundation 생성에 실패한 경우 예외를 throw합니다.
		}

#ifdef _DEBUG
		// PhysX Visual Debugger (PVD)를 생성하고 설정합니다.
		mPvd = physx::PxCreatePvd(*mFoundation); // Foundation을 사용하여 PVD를 생성합니다.
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10); // PVD에 사용할 트랜스포트를 생성합니다.
		mPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL); // PVD를 트랜스포트에 연결합니다.

		// 물리 시뮬레이션의 허용 오차 스케일을 설정합니다.
		mToleranceScale.length = 100; // 길이 허용 오차 스케일을 설정합니다.
		mToleranceScale.speed = 981; // 속도 허용 오차 스케일을 설정합니다.

		// PhysX Physics를 생성하고 초기화합니다.
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd); // Physics를 생성합니다.

		// CPU 디스패처를 생성하고 설정합니다.
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(3); // CPU 디스패처를 생성합니다.
#else
		// 물리 시뮬레이션의 허용 오차 스케일을 설정합니다.
		mToleranceScale.length = 100; // 길이 허용 오차 스케일을 설정합니다.
		mToleranceScale.speed = 981; // 속도 허용 오차 스케일을 설정합니다.

		// PhysX Physics를 생성하고 초기화합니다.
		mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, nullptr); // Physics를 생성합니다.

		// CPU 디스패처를 생성하고 설정합니다.
		mDispatcher = physx::PxDefaultCpuDispatcherCreate(2); // CPU 디스패처를 생성합니다.
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
