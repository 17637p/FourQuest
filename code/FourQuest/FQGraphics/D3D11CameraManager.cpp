#include "D3D11CameraManager.h"

#include <FQCommonGraphics.h>

#include "Camera.h"

fq::graphics::D3D11CameraManager::D3D11CameraManager()
	:mCameras{}
{
}

void fq::graphics::D3D11CameraManager::AddCamera(const ECameraType& cameraType, const unsigned short width, const unsigned short height, const CameraInfo& cameraInfo)
{
	std::shared_ptr<Camera> tempCamera = std::make_shared<Camera>();
	tempCamera->Initialize(width, height, cameraInfo);
	mCameras[cameraType] = tempCamera;
}

void fq::graphics::D3D11CameraManager::Update(const ECameraType& cameraType, const fq::common::Transform& transform)
{
	mCameras[ECameraType::Player]->Update(transform);
}

void fq::graphics::D3D11CameraManager::SetCamera(const ECameraType& cameraType, const CameraInfo& cameraInfo)
{
	mCameras[ECameraType::Player]->SetCamera(cameraInfo);
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

