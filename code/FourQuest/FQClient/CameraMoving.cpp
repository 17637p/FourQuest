#include "CameraMoving.h"

#include "Player.h"

fq::client::CameraMoving::CameraMoving()
	:mMainCamera(nullptr),
	mPlayerTransforms{},
	mIsFixed{false},
	mCurZoom(10),
	mIsZoomIn(false),
	mIsZoomOut(false),
	mMoveSpeed(15.f),
	mZoomSpeed(3.f),
	mZoomMin(5),
	mZoomMax(30),
	mZoomOutPadX{-0.8f, 0.8f},
	mZoomOutPadY{-0.7f, 1.0f},
	mZoomInPadX{ -0.4f, 0.4f },
	mZoomInPadY{ -0.3f, 0.6f }
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
	// 플레이어 중앙 추적
	chaseCenter(dt);
	if (GetAsyncKeyState('O') & 0x8000)
	{
		mIsFixed = !mIsFixed;
	}
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::getCenterPointInView(float dt)
{
	// 등록된 플레이어가 없으면 
	if (mPlayerTransforms.size() == 0 || mIsFixed)
	{
		spdlog::error("There are no players registered on the camera");
		return { 0, 0, mCurZoom };
	}

	// width height 어떻게 가져옴 
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();// * mMainCamera->GetProjection(2068.f/898.f);

	// 플레이어 트랜스폼 돌면서 센터 점 계산 
	DirectX::SimpleMath::Vector3 playersCenterPoint = { 0, 0, 0 };
	mIsZoomIn = true;
	mIsZoomOut = false;
	for (const auto& playerTransform : mPlayerTransforms)
	{
		DirectX::SimpleMath::Vector3 playerWorldPosition = playerTransform->GetWorldPosition();
		playerWorldPosition = DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, viewMatrix);
		playersCenterPoint += playerWorldPosition;

		playerWorldPosition = DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, mMainCamera->GetProjection(2068.f / 898.f));
		//spdlog::trace("{}, {}, {}", playerWorldPosition.x, playerWorldPosition.y, playerWorldPosition.z);

		if (playerWorldPosition.x > mZoomOutPadX.y || playerWorldPosition.x < mZoomOutPadX.x)
		{
			mIsZoomOut = true;
		}
		if (playerWorldPosition.y > mZoomOutPadX.y || playerWorldPosition.y < mZoomOutPadX.x)
		{
			mIsZoomOut = true;
		}
		if (playerWorldPosition.x > mZoomInPadX.y || playerWorldPosition.x < mZoomInPadX.x)
		{
			mIsZoomIn = false;
		}
		if (playerWorldPosition.y > mZoomInPadY.y || playerWorldPosition.y < mZoomInPadY.x)
		{
			mIsZoomIn = false;
		}
	}

	if (mIsZoomIn)
	{
		zoomIn(dt);
	}
	if (mIsZoomOut)
	{
		zoomOut(dt);
	}

	playersCenterPoint /= mPlayerTransforms.size();
	//spdlog::trace("{}, {}, {}", playersCenterPoint.x, playersCenterPoint.y, playersCenterPoint.z);

	return playersCenterPoint;
}

void fq::client::CameraMoving::OnStart()
{
	mMainCamera = GetComponent<fq::game_module::Camera>();
}

void fq::client::CameraMoving::chaseCenter(float dt)
{
	fq::game_module::Transform* myTransform = this->GetGameObject()->GetComponent<fq::game_module::Transform>();

	DirectX::SimpleMath::Vector3 center = getCenterPointInView(dt); // 방향 벡터로 사용
	DirectX::SimpleMath::Vector3 centerCopy = center; // 거리 재는 용

	spdlog::trace("{}, {}, {}", center.x, center.y, center.z);
	// 0,0 밖에 있따면 움직이기 
	float epsilon = 0.001f;
	if (abs(center.x) > epsilon || abs(center.y) > epsilon)
	{
		center.Normalize();
	}

	// 카메라의 현재 축 구하기 
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();
	DirectX::SimpleMath::Vector3 cameraRight = { viewMatrix._11, viewMatrix._12, viewMatrix._13 };
	// 왜 둘이 반대지?
	DirectX::SimpleMath::Vector3 cameraUp = { viewMatrix._31, viewMatrix._32, viewMatrix._33 };
	DirectX::SimpleMath::Vector3 cameraForward = { viewMatrix._21, viewMatrix._22, viewMatrix._23 };

	// X 방향 이동 값
	DirectX::SimpleMath::Vector3 rightMove{};
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

	// y 방향 이동 값
	DirectX::SimpleMath::Vector3 upMove{};
	if (abs(centerCopy.y) < center.y * mMoveSpeed * dt)
	{
		upMove = cameraUp * centerCopy.y;
	}
	else
	{
		upMove = cameraUp * (center.y * mMoveSpeed * dt);
	}

	// z 방향 이동 값
	DirectX::SimpleMath::Vector3 forwardMove{};
	if (abs(centerCopy.z) - mCurZoom < center.z * mMoveSpeed * dt)
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
}

void fq::client::CameraMoving::zoomIn(float dt)
{
	if (mCurZoom > mZoomMin)
	{
		mCurZoom -= mZoomSpeed * dt;
	}
}

void fq::client::CameraMoving::zoomOut(float dt)
{
	if (mCurZoom < mZoomMax)
	{
		mCurZoom += mZoomSpeed * dt;
	}
}

void fq::client::CameraMoving::AddPlayerTransform(fq::game_module::Transform* player)
{
	mPlayerTransforms.push_back(player);
}

void fq::client::CameraMoving::DeletePlayerTransform(fq::game_module::Transform* player)
{
	mPlayerTransforms.erase(std::remove(mPlayerTransforms.begin(), mPlayerTransforms.end(), player), mPlayerTransforms.end());
}
