#include "CameraSystem.h"

#include <spdlog/spdlog.h>

#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"

fq::game_engine::CameraSystem::CameraSystem()
	:mGameProcess(nullptr)
	, mSetMainCameraHandler{}
	, mOnLoadedSceneHandler{}
	, mGameMainCamera(nullptr)
	, mEditorCamera(nullptr)
	, mCameraType(CameraType::None)
{}

fq::game_engine::CameraSystem::~CameraSystem()
{

}

void fq::game_engine::CameraSystem::Initialize(GameProcess* gameProcess)
{
	mGameProcess = gameProcess;

	// 이벤트 핸들 등록 
	mSetMainCameraHandler = mGameProcess->mEventManager->RegisterHandle<fq::event::SetMainCamera>(
		[this](fq::event::SetMainCamera event)
		{
			SetMainGameCamera(event.mainCamera);
		});

	mOnLoadedSceneHandler = mGameProcess->mEventManager->RegisterHandle<fq::event::OnLoadScene>(
		[this](fq::event::OnLoadScene event)
		{
			SetMainGameCamera(FindMainCamera());
			if (!mGameMainCamera) SPDLOG_WARN("Can't Find MainCamera");
		});
}

void fq::game_engine::CameraSystem::SetMainGameCamera(fq::game_module::Camera* camera)
{
	mGameMainCamera = camera;
}

void fq::game_engine::CameraSystem::SetEditorCamera(fq::game_module::Camera* camera)
{
	mEditorCamera = camera;
}

void fq::game_engine::CameraSystem::SetBindCamera(CameraType type)
{
	mCameraType = type;

	if (type == CameraType::Game)
	{
		mGameProcess->mGraphics->SetCamera(mGameMainCamera->GetCameraInfomation());
	}
	else if (type == CameraType::Editor)
	{
		mGameProcess->mGraphics->SetCamera(mEditorCamera->GetCameraInfomation());
	}
}

void fq::game_engine::CameraSystem::Update()
{
	if (mCameraType == CameraType::Game)
	{
		auto transform = mGameMainCamera->GetComponent<fq::game_module::Transform>()->GetTransform();
		mGameProcess->mGraphics->UpdateCamera(transform);
	}
	else if (mCameraType == CameraType::Editor)
	{
		auto trasform = mEditorCamera->GetComponent<fq::game_module::Transform>()->GetTransform();
		mGameProcess->mGraphics->UpdateCamera(trasform);
	}
	else
	{
		// 초기화 필요
		assert(nullptr);
	}
}

fq::game_module::Camera* fq::game_engine::CameraSystem::FindMainCamera() const
{
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();

	for ( auto& object : scene->GetComponentView<fq::game_module::Camera>())
	{
		auto camera =  object.GetComponent<fq::game_module::Camera>();

		if (camera->IsMain())
		{
			return camera;
		}
	}

	return nullptr;
}
