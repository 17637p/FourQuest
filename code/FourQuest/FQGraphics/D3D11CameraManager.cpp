#include "D3D11CameraManager.h"

#include <FQCommonGraphics.h>

#include "Camera.h"

fq::graphics::D3D11CameraManager::D3D11CameraManager()
	:mCameras{},
	mOnSetCamera{}
{
	// Todo: 일단 수동으로 해주고 나중에 바꾸자
	std::shared_ptr<Camera> tempCamera = std::make_shared<Camera>();
	mCameras[ECameraType::Player] = tempCamera;
}

void fq::graphics::D3D11CameraManager::Update(const ECameraType& cameraType, const fq::common::Transform& transform)
{
	mCameras[ECameraType::Player]->Update(transform);
}

void fq::graphics::D3D11CameraManager::SetCamera(const ECameraType& cameraType, const CameraInfo& cameraInfo)
{
	mCameras[ECameraType::Player]->SetCamera(cameraInfo);

	for (const auto& event : mOnSetCamera)
	{
		event();
	}
}

void fq::graphics::D3D11CameraManager::SetViewportSize(const ECameraType& cameraType, const unsigned short width, const unsigned short height)
{
	mCameras[ECameraType::Player]->SetViewportSize(width, height);
}

DirectX::SimpleMath::Matrix fq::graphics::D3D11CameraManager::GetViewMatrix(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetViewMatrix();
}

DirectX::SimpleMath::Matrix fq::graphics::D3D11CameraManager::GetProjectionMatrix(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetProjectionMatrix();
}

DirectX::SimpleMath::Vector3 fq::graphics::D3D11CameraManager::GetPosition(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetPosition();
}

DirectX::SimpleMath::Quaternion fq::graphics::D3D11CameraManager::GetRotation(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetRotation();
}

float fq::graphics::D3D11CameraManager::GetNearPlane(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetNearPlane();
}

float fq::graphics::D3D11CameraManager::GetFarPlane(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetFarPlane();
}

void fq::graphics::D3D11CameraManager::Initialize(const unsigned short width, const unsigned short height)
{
	OnResize(width, height);
}

void fq::graphics::D3D11CameraManager::OnResize(const unsigned short width, const unsigned short height)
{
	for (const auto& camera : mCameras)
	{
		camera.second->SetViewportSize(width, height);
	}
}

float fq::graphics::D3D11CameraManager::GetFovY(const ECameraType& cameraType) const
{
	return mCameras.at(ECameraType::Player)->GetFovY();
}

void fq::graphics::D3D11CameraManager::RegisterOnSetCamera(std::function<void()> onSetCameraEvent)
{
	mOnSetCamera.push_back(onSetCameraEvent);
}

