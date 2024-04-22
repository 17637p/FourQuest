#include "Camera.h"

#include "Scene.h"


fq::game_module::Camera::Camera()
	:mbIsMain(false)
	,mCameraInfomation{}
{
	SetNearPlain(0.3f);
	SetFarPlain(1000.f);
	SetFieldOfView(60);
}

fq::game_module::Camera::~Camera()
{

}

fq::game_module::Component* fq::game_module::Camera::Clone(Component* clone /* = nullptr */) const
{
	Camera* cloneCamera = static_cast<Camera*>(clone);

	if (cloneCamera == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCamera = new Camera(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCamera = *this;
	}
	return cloneCamera;
}

entt::meta_handle fq::game_module::Camera::GetHandle()
{
	return *this;
}


void fq::game_module::Camera::SetMainCamera(bool bIsMain)
{
	// �ٸ� ���� ī�޶�� �����մϴ�
	if (bIsMain)
	{
		GetScene()->ViewComponents<Camera>([this](GameObject& object, Camera& camera)
			{
				camera.SetMainCamera(false);
			});
	}

	mbIsMain = bIsMain;
}

void fq::game_module::Camera::SetFieldOfView(float degree)
{
	constexpr float min = DirectX::XMConvertToRadians(0.00001f);
	constexpr float max = DirectX::XMConvertToRadians(180.f);

	mCameraInfomation.filedOfView = 
		std::clamp(DirectX::XMConvertToRadians(degree), min, max);
}

float fq::game_module::Camera::GetFieldOfView() const
{
	return DirectX::XMConvertToDegrees(mCameraInfomation.filedOfView);
}

float fq::game_module::Camera::GetNearPlain() const
{
	return mCameraInfomation.nearPlain;
}


float fq::game_module::Camera::GetFarPlain() const
{
	return mCameraInfomation.farPlain;
}

void fq::game_module::Camera::SetNearPlain(float distance) 
{
	mCameraInfomation.nearPlain = distance;
}

void fq::game_module::Camera::SetFarPlain(float distance)
{
	mCameraInfomation.farPlain = distance;
}
