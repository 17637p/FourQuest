#include "Camera.h"

#include <FQCommon.h>

fq_graphics::Camera::Camera()
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

fq_graphics::Camera::~Camera()
{

}

void fq_graphics::Camera::Initialize(const unsigned short width, const unsigned short height, const CameraInfo& cameraInfo)
{
	SetViewportSize(width, height);
	SetCamera(cameraInfo);
}

void fq_graphics::Camera::Update(const fq::Transform& transform)
{
	mPosition = transform.worldPosition;
	mRotation = transform.worldRotation;

	mViewMatrix = transform.worldMatrix.Invert();
}

void fq_graphics::Camera::SetCamera(const CameraInfo& cameraInfo)
{
	mIsPerspective = cameraInfo.isPerspective;
	mFieldOfView = cameraInfo.filedOfView;
	mNearPlain = cameraInfo.nearPlain;
	mFarPlain = cameraInfo.farPlain;

	makeProjectionMatrix();
}

void fq_graphics::Camera::SetViewportSize(const unsigned short width, const unsigned short height)
{
	mWidth = width;
	mHeight = height;

	makeProjectionMatrix();
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

void fq_graphics::Camera::makeProjectionMatrix()
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
