#include "Camera.h"

#include <FQCommon.h>
#include <FQCommonGraphics.h>

fq::graphics::Camera::Camera()
	:mWidth{0},
	mHeight{0},
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
	mFieldOfView = cameraInfo.filedOfView;
	mNearPlain = cameraInfo.nearPlain;
	mFarPlain = cameraInfo.farPlain;

	makeProjectionMatrix();
}

void fq::graphics::Camera::SetViewportSize(const unsigned short width, const unsigned short height)
{
	mWidth = width;
	mHeight = height;

	makeProjectionMatrix();
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

void fq::graphics::Camera::makeProjectionMatrix()
{
	if (mIsPerspective)
	{
		mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(mFieldOfView, static_cast<float>(mWidth) / mHeight, mNearPlain, mFarPlain);
	}
	else
	{
		mProjectionMatrix = DirectX::XMMatrixOrthographicLH(static_cast<float>(mWidth), static_cast<float>(mHeight), mNearPlain, mFarPlain);
	}
}
