#include "CameraMoving.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Camera.h"
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

	if (cloneCamera == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneCamera = game_module::ObjectPool::GetInstance()->Assign<CameraMoving>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneCamera = *this;
	}
	cloneCamera->mMainCamera = nullptr;

	return cloneCamera;
}

void fq::client::CameraMoving::OnUpdate(float dt)
{
	// �÷��̾� �߾� ����
	chaseCenter(dt);
	if (GetAsyncKeyState('O') & 0x8000)
	{
		//mIsFixed = !mIsFixed;
	}
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::getCenterPointInView(float dt)
{
	// ��ϵ� �÷��̾ ������ 
	if (mPlayerTransforms.size() == 0 || mIsFixed)
	{
		spdlog::error("There are no players registered on the camera");
		return { 0, 0, mCurZoom };
	}

	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();

	// �÷��̾� Ʈ������ ���鼭 ���� �� ��� 
	mPlayersCenterPoint = { 0, 0, 0 };
	mIsZoomIn = true;
	mIsZoomOut = false;

	for (const auto& playerTransform : mPlayerTransforms)
	{
		DirectX::SimpleMath::Vector3 playerWorldPosition = playerTransform->GetWorldPosition();
		playerWorldPosition = DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, viewMatrix);
		mPlayersCenterPoint += playerWorldPosition;

		//playerWorldPosition = DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, mMainCamera->GetProjection(2068.f / 898.f));
		float aspectRatio = mMainCamera->GetAspectRatio();
		playerWorldPosition = DirectX::SimpleMath::Vector3::Transform(playerWorldPosition, mMainCamera->GetProjection(aspectRatio));
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

	// �÷��̾� ��ġ �ݿ��ؼ� Zoom ó��
	if (mIsZoomIn)
	{
		zoomIn(dt);
	}
	if (mIsZoomOut)
	{
		zoomOut(dt);
	}

	mPlayersCenterPoint /= mPlayerTransforms.size();
	//spdlog::trace("{}, {}, {}", playersCenterPoint.x, playersCenterPoint.y, playersCenterPoint.z);

	return mPlayersCenterPoint;
}

void fq::client::CameraMoving::OnStart()
{
	mMainCamera = GetComponent<fq::game_module::Camera>();
}

void fq::client::CameraMoving::chaseCenter(float dt)
{
	fq::game_module::Transform* myTransform = this->GetGameObject()->GetComponent<fq::game_module::Transform>();

	DirectX::SimpleMath::Vector3 center = getCenterPointInView(dt); // ���� ���ͷ� ���
	DirectX::SimpleMath::Vector3 centerCopy = center; // �Ÿ� ��� ��

	//spdlog::trace("{}, {}, {}", center.x, center.y, center.z);
	// 0,0 �ۿ� �ֵ��� �����̱� 
	float epsilon = 0.001f;
	if (abs(center.x) > epsilon || abs(center.y) > epsilon)
	{
		center.Normalize();
	}

	// ī�޶��� ���� �� ���ϱ� 
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();
	DirectX::SimpleMath::Vector3 cameraRight = { viewMatrix._11, viewMatrix._12, viewMatrix._13 };
	// �� ���� �ݴ���?
	DirectX::SimpleMath::Vector3 cameraUp = { viewMatrix._31, viewMatrix._32, viewMatrix._33 };
	DirectX::SimpleMath::Vector3 cameraForward = { viewMatrix._21, viewMatrix._22, viewMatrix._23 };

	// X ���� �̵� ��
	DirectX::SimpleMath::Vector3 rightMove{};
	// ������ �Ÿ����� center���� �Ÿ��� ������ 
	if (abs(centerCopy.x) < center.x * mMoveSpeed * dt)
	{
		// center�� �ٷ� �����̱�
		rightMove = cameraRight * centerCopy.x;
	}
	else
	{
		// center�� ���ؼ� �����̱� 
		rightMove = cameraRight * (center.x * mMoveSpeed * dt);
	}

	// y ���� �̵� ��
	DirectX::SimpleMath::Vector3 upMove{};
	if (abs(centerCopy.y) < center.y * mMoveSpeed * dt)
	{
		upMove = cameraUp * centerCopy.y;
	}
	else
	{
		upMove = cameraUp * (center.y * mMoveSpeed * dt);
	}

	// z ���� �̵� ��
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

	// ī�޶� Ʈ�������� ����
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

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::GetCenterCameraInWorld()
{
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();
	return DirectX::SimpleMath::Vector3::Transform(mPlayersCenterPoint, viewMatrix.Invert());
}
