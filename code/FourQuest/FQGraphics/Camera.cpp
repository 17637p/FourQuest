#include "Camera.h"

#include <FQCommon.h>
#include <FQCommonGraphics.h>

fq::graphics::Camera::Camera()
	:mWidth{ 0 },
	mHeight{ 0 },
	mIsPerspective{ true },
	mFieldOfView{ 0 },
	mNearPlane{ 0 },
	mFarPlane{ 0 },
	mViewMatrix{},
	mProjectionMatrix{},
	mPosition{},
	mRotation{}
{

}

fq::graphics::Camera::~Camera()
{

}

void fq::graphics::Camera::Initialize(const unsigned short width, const unsigned short height, const CameraInfo& cameraInfo)
{
	SetViewportSize(width, height);
	SetCamera(cameraInfo);
}

void fq::graphics::Camera::Update(const fq::common::Transform& transform)
{
	mPosition = transform.worldPosition;
	mRotation = transform.worldRotation;

	mViewMatrix = transform.worldMatrix.Invert();
}

void fq::graphics::Camera::SetCamera(const CameraInfo& cameraInfo)
{
	mIsPerspective = cameraInfo.isPerspective;
	mFieldOfView = cameraInfo.fieldOfView;
	mNearPlane = cameraInfo.nearPlain;
	mFarPlane = cameraInfo.farPlain;

	makeProjectionMatrix();
}

void fq::graphics::Camera::SetViewportSize(const unsigned short width, const unsigned short height)
{
	mWidth = width;
	mHeight = height;

	if (mFarPlane > 0)
	{
		makeProjectionMatrix();
	}
}

DirectX::SimpleMath::Matrix fq::graphics::Camera::GetViewMatrix() const
{
	return mViewMatrix;
}

DirectX::SimpleMath::Matrix fq::graphics::Camera::GetProjectionMatrix() const
{
	return mProjectionMatrix;
}

DirectX::SimpleMath::Vector3 fq::graphics::Camera::GetPosition() const
{
	return mPosition;
}

DirectX::SimpleMath::Quaternion fq::graphics::Camera::GetRotation() const
{
	return mRotation;
}

float fq::graphics::Camera::GetNearPlane() const
{
	return mNearPlane;
}

float fq::graphics::Camera::GetFarPlane() const
{
	return mFarPlane;
}

void fq::graphics::Camera::makeProjectionMatrix()
{
	if (mIsPerspective)
	{
		mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFieldOfView, static_cast<float>(mWidth) / mHeight, mNearPlane, mFarPlane);
	}
	else
	{
		mProjectionMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(mWidth), static_cast<float>(mHeight), mNearPlane, mFarPlane);
	}
}

float fq::graphics::Camera::GetFovY() const
{
	return mFieldOfView;
}
