#include "Camera.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"
#include "Transform.h"

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

std::shared_ptr<fq::game_module::Component> fq::game_module::Camera::Clone(std::shared_ptr<Component> clone /*= nullptr*/) const
{
	auto cloneCamera = std::dynamic_pointer_cast<Camera>(clone);

	if (cloneCamera == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCamera = ObjectPool::GetInstance()->Assign<Camera>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
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
	if (bIsMain && GetGameObject())
	{
		GetScene()->ViewComponents<Camera>([this](GameObject& object, Camera& camera)
			{
				camera.SetMainCamera(false);
			});

		GetScene()->GetEventManager()->FireEvent<fq::event::SetMainCamera>({this});
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

DirectX::SimpleMath::Matrix fq::game_module::Camera::GetProjection(float aspectRatio) const
{
	return DirectX::XMMatrixPerspectiveFovLH(mCameraInfomation.filedOfView
		, aspectRatio
		, mCameraInfomation.nearPlain
		, mCameraInfomation.farPlain);

}
DirectX::SimpleMath::Matrix fq::game_module::Camera::GetView()
{
	auto transform = GetComponent<Transform>();

	return transform->GetWorldMatrix().Invert();
}

