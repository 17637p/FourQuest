#include "CameraSystem.h"

#include "GameProcess.h"

fq::game_engine::CameraSystem::CameraSystem()
	:mGameProcess(nullptr)
{}

fq::game_engine::CameraSystem::~CameraSystem()
{

}

void fq::game_engine::CameraSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// ����ī�޶� ����
	mGameProcess->mEventManager->RegisterHandle<fq::event::SetMainCamera>(
		[this](fq::event::SetMainCamera event)
		{
			this->mMainCamera = event.mainCamera;
		}
	);

}
