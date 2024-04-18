#include "Camera.h"

#include <FQCommon.h>

fq_graphics::Camera::Camera(unsigned short width, unsigned short height)
	:mWidth{width},
	mHeight{height},
	mIsPerspective{true},
	mFieldOfView{0},
	mNearPlain{0},
	mFarPlain{0},
	mViewMatrix{},
	mProjectionMatrix{},
	mPosition{},
	mRotation{}
{
}

fq_graphics::Camera::~Camera()
{

}

void fq_graphics::Camera::Update(const fq::Transform& transform)
{
	mPosition = transform.worldPosition;
}

void fq_graphics::Camera::SetCamera(const CameraInfo& cameraInfo)
{

}

void fq_graphics::Camera::SetViewportSize(int width, int height)
{

}

DirectX::SimpleMath::Matrix fq_graphics::Camera::GetViewMatrix() const
{
	return mViewMatrix;
}

DirectX::SimpleMath::Matrix fq_graphics::Camera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

DirectX::SimpleMath::Vector3 fq_graphics::Camera::GetPosition() const
{
	return mPosition;
}

DirectX::SimpleMath::Quaternion fq_graphics::Camera::GetRotation() const
{
	return mRotation;
}
