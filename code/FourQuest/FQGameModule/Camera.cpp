#include "Camera.h"

#include "Scene.h"


fq::game_module::Camera::Camera()
	:mbIsMain(false)
	,mCameraInfomation{}
{
	mCameraInfomation.isPerspective = true;
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

	if (cloneCamera == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCamera = new Camera(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
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
	// 다른 메인 카메라는 해제합니다
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

