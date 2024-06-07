#include "CameraMoving.h"

#include "Player.h"

fq::client::CameraMoving::CameraMoving()
	:mMainCamera(nullptr),
	mMoveSpeed(15.f),
	mCurZoom(10),
	mZoomMin(5),
	mZoomMax(30)
{

}

fq::client::CameraMoving::~CameraMoving()
{
}

std::shared_ptr<fq::game_module::Component> fq::client::CameraMoving::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<CameraMoving> cloneCamera = std::dynamic_pointer_cast<CameraMoving>(clone);

	if (cloneCamera == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCamera = game_module::ObjectPool::GetInstance()->Assign<CameraMoving>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCamera = *this;
	}
	cloneCamera->mMainCamera = nullptr;

	return cloneCamera;
}

void fq::client::CameraMoving::OnUpdate(float dt)
{
	DirectX::SimpleMath::Vector3 center = GetCenterPoint(); // 방향 벡터로 사용
	DirectX::SimpleMath::Vector3 centerCopy = center; // 거리 재는 용

	// 0,0 밖에 있따면 움직이기 
	float epsilon = 0.001f;
	if (abs(center.x) > epsilon || abs(center.y) > epsilon)
	{
		center.Normalize();
	}

	// center 위치와 카메라가 움직일 방향은 반대이므로

	// 카메라의 현재 축 구하기 
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();
	DirectX::SimpleMath::Vector3 cameraRight = { viewMatrix._11, viewMatrix._12, viewMatrix._13 };
	// 왜 둘이 반대지?
	DirectX::SimpleMath::Vector3 cameraUp = { viewMatrix._31, viewMatrix._32, viewMatrix._33 };
	DirectX::SimpleMath::Vector3 cameraForward = { viewMatrix._21, viewMatrix._22, viewMatrix._23 };

	// X 방향 이동 값
	DirectX::SimpleMath::Vector3 rightMove{};
	// y 방향 이동 값
	DirectX::SimpleMath::Vector3 upMove{};
	// z 방향 이동 값
	DirectX::SimpleMath::Vector3 forwardMove{};

	// 움직일 거리보다 center와의 거리가 작으면 
	if (abs(centerCopy.x) < center.x * mMoveSpeed * dt)
	{
		// center로 바로 움직이기
		rightMove = cameraRight * centerCopy.x;
	}
	else
	{
		// center를 향해서 움직이기 
		rightMove = cameraRight * (center.x * mMoveSpeed * dt);
	}

	if (abs(centerCopy.y) < center.y * mMoveSpeed * dt)
	{
		upMove = cameraUp * centerCopy.y;
	}
	else
	{
		upMove = cameraUp * (center.y * mMoveSpeed * dt);
	}

	if (abs(centerCopy.z) - mCurZoom < mMoveSpeed * dt)
	{
		forwardMove = -cameraForward * (centerCopy.z - mCurZoom);
	}
	else
	{
		if (abs(abs(centerCopy.z) - mCurZoom) > epsilon)
		{
			forwardMove = -cameraForward * center.z * mMoveSpeed * dt;
		}
	}

	// 카메라 트랜스폼에 적용
	auto cameraTransform = mMainCamera->GetGameObject()->GetComponent<fq::game_module::Transform>();
	cameraTransform->SetLocalPosition(cameraTransform->GetLocalPosition() + rightMove + upMove + forwardMove);

	auto inputMgr = GetScene()->GetInputManager();
	if (inputMgr->IsKeyState(EKey::A, EKeyState::Tap))
	{
		//cameraTransform->SetLocalPosition(cameraTransform->GetLocalPosition() + cameraUp);
	}

	// Max, min 줌
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::GetCenterPoint() const
{
	// 플레이어 트랜스폼 가져오기
	std::vector<fq::game_module::Transform*> playersTransform;

	auto componentView = GetScene()->GetComponentView<Player>();
	for (auto& player : componentView)
	{
		playersTransform.push_back(player.GetComponent<fq::game_module::Transform>());
	}

	// width height 어떻게 가져옴 
	DirectX::SimpleMath::Matrix viewProjectionMatrix = mMainCamera->GetView();// * mMainCamera->GetProjection(2068.f/898.f);

	// 센터 점 계산 
	DirectX::SimpleMath::Vector3 playersCenterPoint = { 0, 0, 0 };
	for (const auto& playerTransform : playersTransform)
	{
		DirectX::SimpleMath::Vector3 playerWorldPosition = playerTransform->GetWorldPosition();
		playersCenterPoint += DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, viewProjectionMatrix);
	}

	playersCenterPoint /= playersTransform.size();
	spdlog::trace("{}, {}, {}", playersCenterPoint.x, playersCenterPoint.y, playersCenterPoint.z);

	return playersCenterPoint;
}

void fq::client::CameraMoving::OnStart()
{
	mMainCamera = GetComponent<fq::game_module::Camera>();
}
