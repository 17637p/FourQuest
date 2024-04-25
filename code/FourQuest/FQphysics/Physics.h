#pragma once

#include <physx/PxPhysicsAPI.h>

#include <memory>

namespace fq::physics
{
	class Physics
	{
	public:
		Physics();
		~Physics();

		/// <summary>
		/// �⺻ PhysX�� �����մϴ�.
		/// </summary>
		void Initialize();
		
		/// <summary>
		/// PhysX Visual Debugger ���α׷��� ������ ���� ������ �����մϴ�.
		/// </summary>
		/// <param name="scene"></param>
		void SettingPVDClient(physx::PxScene* scene);

		/// <summary>
		/// Physics�� ��ȯ�մϴ�.
		/// </summary>
		inline physx::PxPhysics* GetPhysics();
		
		/// <summary>
		/// CpuDispatcher�� ��ȯ�մϴ�.
		/// </summary>
		inline physx::PxDefaultCpuDispatcher* GetDispatcher();

	private:
		physx::PxDefaultAllocator		mDefaultAllocatorCallback;
		physx::PxDefaultErrorCallback	mDefaultErrorCallback;
		physx::PxTolerancesScale		mToleranceScale;
		physx::PxDefaultCpuDispatcher*	mDispatcher;

		// physx
		physx::PxFoundation*			mFoundation;
		physx::PxPhysics*				mPhysics;

		// PVD
		physx::PxPvd*					mPvd;
		physx::PxPvdSceneClient*		mPvdClient;
	};

	physx::PxPhysics* Physics::GetPhysics()
	{
		return mPhysics;
	}
	physx::PxDefaultCpuDispatcher* Physics::GetDispatcher()
	{
		return mDispatcher;
	}
}


