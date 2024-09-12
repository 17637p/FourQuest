#include "CameraMoving.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Camera.h"

// Test
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/ScreenManager.h"

#include "Player.h"

fq::client::CameraMoving::CameraMoving()
	:mMainCamera(nullptr),
	mPlayerTransforms{},
	mIsFixed{ false },
	mCurZoom(10),
	mIsZoomIn(false),
	mIsZoomOut(false),
	mMoveSpeed(15.f),
	mZoomSpeed(3.f),
	mZoomMin(5),
	mZoomMax(30),
	mZoomOutPadX{ -0.7f, 0.7f },
	mZoomOutPadY{ -0.5f, 0.9f },
	mZoomInPadX{ -0.4f, 0.4f },
	mZoomInPadY{ -0.3f, 0.6f },
	mForbiddenAreaPaddingX{ -0.85f, 0.85f },
	mForbiddenAreaPaddingY{ -0.7f, 1.0f }
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
	if (mPlayerTransforms.size() <= 0)
	{
		return;
	}

	// �÷��̾� �߾� ����
	chaseCenter(dt);
	restrcitPlayerMove();

	/*auto input = GetScene()->GetInputManager();
	if (input->GetKeyState(EKey::A) == EKeyState::Tap)
	{
		bool isis = IsValid(0, EDirection::Left);
		if (isis)
		{
			spdlog::trace("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
		}
		else
		{
			spdlog::trace("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		}
	}
	if (input->GetKeyState(EKey::W) == EKeyState::Tap)
	{
		bool isis = IsValid(0, EDirection::Top);
		if (isis)
		{
			spdlog::trace("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
		}
		else
		{
			spdlog::trace("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		}
	}
	if (input->GetKeyState(EKey::S) == EKeyState::Tap)
	{
		bool isis = IsValid(0, EDirection::Bottom);
		if (isis)
		{
			spdlog::trace("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
		}
		else
		{
			spdlog::trace("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		}
	}
	if (input->GetKeyState(EKey::D) == EKeyState::Tap)
	{
		bool isis = IsValid(0, EDirection::Right);
		if (isis)
		{
			spdlog::trace("OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
		}
		else
		{
			spdlog::trace("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		}
	}*/
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::getCenterPosInView(float dt)
{
	// ��ϵ� �÷��̾ ������ 
	if (mPlayerTransforms.size() == 0 || mIsFixed)
	{
		spdlog::error("There are no players registered on the camera");
		return { 0, 0, mCurZoom };
	}

	// �÷��̾� Ʈ������ ���鼭 ���� �� ��� 
	mPlayersCenterPoint = { 0, 0, 0 };

	for (const auto& playerTransform : mPlayerTransforms)
	{
		DirectX::SimpleMath::Vector3 playerWorldPosition = playerTransform->GetWorldPosition();
		mPlayersCenterPoint += getViewPos(playerWorldPosition);
	}

	mPlayersCenterPoint /= mPlayerTransforms.size();
	//spdlog::trace("{}, {}, {}", playersCenterPoint.x, playersCenterPoint.y, playersCenterPoint.z);

	return mPlayersCenterPoint;
}

void fq::client::CameraMoving::OnStart()
{
	mMainCamera = GetComponent<fq::game_module::Camera>();

	InitCameraPos();
	SetColliderRotation();
}

void fq::client::CameraMoving::chaseCenter(float dt)
{
	float maxDT = 1 / (float)60;
	if (dt > maxDT)
	{
		dt = maxDT;
	}

	fq::game_module::Transform* myTransform = this->GetGameObject()->GetComponent<fq::game_module::Transform>();

	Zoom(dt);
	DirectX::SimpleMath::Vector3 center = getCenterPosInView(dt); // ���� ���ͷ� ���

	/// �׽�Ʈ �� 
	float aspectRatio = mMainCamera->GetAspectRatio();
	DirectX::SimpleMath::Matrix projMat = mMainCamera->GetProjection(aspectRatio);

	DirectX::SimpleMath::Vector3 projCenter = DirectX::SimpleMath::Vector3::Transform(center, projMat);

	// �� �Ʒ� ���� (���� ����)
	float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
	float height = GetScene()->GetScreenManager()->GetFixScreenHeight();
	float xViewport = ((projCenter.x + 1) / 2) * width;
	float yViewport = ((1 - projCenter.y) / 2) * height;
	yViewport += 100;
	//center.x = 2 * (xViewport / width) - 1;
	//center.y = 1 - 2 * (yViewport / height);

	DirectX::SimpleMath::Vector3 centerCopy = center; // �Ÿ� ��� ��

	// zoom 20 �������� 100;
	//spdlog::trace("{}, {}, {}", xViewport, yViewport, center.z);

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

	//centerCopy -= cameraUp * 3 * mCurZoom;

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
	auto d = cameraTransform->GetLocalPosition() + rightMove + upMove + forwardMove;
	spdlog::trace("{} {} {}", d.x, d.y, d.z);
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

bool fq::client::CameraMoving::IsValid(int playerID, EDirection eDirection)
{
	int index = -1;
	for (int i = 0; i < mPlayerTransforms.size(); i++)
	{
		int playerIDi = mPlayerTransforms[i]->GetComponent<game_module::CharacterController>()->GetControllerID();
		if (playerIDi == playerID)
		{
			index = i;
			break;
		}
	}

	DirectX::SimpleMath::Vector3 playerWorldPos = mPlayerTransforms[index]->GetWorldPosition();
	DirectX::SimpleMath::Vector3 projPos = getProjPos(playerWorldPos);

	switch (eDirection)
	{
		case fq::client::EDirection::Top:
			if (projPos.y >= mForbiddenAreaPaddingY.y)
			{
				return false;
			}
			break;
		case fq::client::EDirection::Left:
			if (projPos.x <= mForbiddenAreaPaddingX.x)
			{
				return false;
			}
			break;
		case fq::client::EDirection::Bottom:
			if (projPos.y <= mForbiddenAreaPaddingY.x)
			{
				return false;
			}
			break;
		case fq::client::EDirection::Right:
			if (projPos.x >= mForbiddenAreaPaddingX.y)
			{
				return false;
			}
			break;
		default:
			break;
	}

	return true;
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::getViewPos(DirectX::SimpleMath::Vector3 worldPos)
{
	DirectX::SimpleMath::Matrix viewMat = mMainCamera->GetView();

	return DirectX::SimpleMath::Vector3::Transform(worldPos, viewMat);
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::getProjPos(DirectX::SimpleMath::Vector3 worldPos)
{
	DirectX::SimpleMath::Vector3 viewPos = getViewPos(worldPos);

	float aspectRatio = mMainCamera->GetAspectRatio();
	DirectX::SimpleMath::Matrix projMat = mMainCamera->GetProjection(aspectRatio);

	return DirectX::SimpleMath::Vector3::Transform(viewPos, projMat);
}

void fq::client::CameraMoving::Zoom(float dt)
{
	mIsZoomIn = true;

	for (const auto& playerTransform : mPlayerTransforms)
	{
		DirectX::SimpleMath::Vector3 playerWorldPos = playerTransform->GetWorldPosition();
		DirectX::SimpleMath::Vector3 playerProjPos = getProjPos(playerWorldPos);

		// �� ���̶� zoomOut ���� ���� zoomOut
		if (playerProjPos.x > mZoomOutPadX.y || playerProjPos.x < mZoomOutPadX.x)
		{
			zoomOut(dt);
		}
		if (playerProjPos.y > mZoomOutPadX.y || playerProjPos.y < mZoomOutPadX.x)
		{
			zoomOut(dt);
		}

		// ��� zoomIn ���� �ȿ� ���� zoomIn
		if (playerProjPos.x > mZoomInPadX.y || playerProjPos.x < mZoomInPadX.x)
		{
			mIsZoomIn = false;
		}
		if (playerProjPos.y > mZoomInPadY.y || playerProjPos.y < mZoomInPadY.x)
		{
			mIsZoomIn = false;
		}
	}

	// �÷��̾� ��ġ �ݿ��ؼ� Zoom ó��
	if (mIsZoomIn)
	{
		zoomIn(dt);
	}
}

void fq::client::CameraMoving::SetColliderRotation()
{
	if (GetTransform()->GetChildren().size() > 0)
	{
		DirectX::BoundingFrustum frustum;
		DirectX::BoundingFrustum::CreateFromMatrix(frustum, mMainCamera->GetProjection(mMainCamera->GetAspectRatio()));

		DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();

		// ���Ʒ�
		float yRadian = std::atanf(frustum.TopSlope);
		DirectX::SimpleMath::Vector3 xAxis = { 1, 0, 0 };

		DirectX::SimpleMath::Quaternion rotationTopQuaternion =
			DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(xAxis, -yRadian);
		DirectX::SimpleMath::Quaternion rotationBottomQuaternion =
			DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(xAxis, yRadian);

		GetTransform()->GetChildren()[0]->SetLocalRotation(rotationTopQuaternion);
		GetTransform()->GetChildren()[1]->SetLocalRotation(rotationBottomQuaternion);

		// �¿�
		float xRadian = std::atanf(frustum.LeftSlope);
		DirectX::SimpleMath::Vector3 yAxis = { 0, 1, 0 };

		DirectX::SimpleMath::Quaternion rotationLeftQuaternion =
			DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(yAxis, xRadian);
		DirectX::SimpleMath::Quaternion rotationRightQuaternion =
			DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(yAxis, -xRadian);

		GetTransform()->GetChildren()[2]->SetLocalRotation(rotationLeftQuaternion);
		GetTransform()->GetChildren()[3]->SetLocalRotation(rotationRightQuaternion);
	}
	else
	{
		spdlog::error({ "Please Change Camera Prefab" });
	}
}

void fq::client::CameraMoving::restrcitPlayerMove()
{
	float minY = FLT_MAX;
	float maxY = FLT_MIN;
	int minIndex = -1;
	int maxIndex = -1;
	for (int i = 0; i < mPlayerTransforms.size(); i++)
	{
		DirectX::SimpleMath::Vector3 playerWorldPos = mPlayerTransforms[i]->GetWorldPosition();
		DirectX::SimpleMath::Vector3 projPos = getProjPos(playerWorldPos);

		if (projPos.y > maxY)
		{
			maxY = projPos.y;
			maxIndex = i;
		}
		if (projPos.y < minY)
		{
			minY = projPos.y;
			minIndex = i;
		}
	}

	for (int i = 0; i < mPlayerTransforms.size(); i++)
	{
		auto controller = mPlayerTransforms[i]->GetComponent<game_module::CharacterController>();
		DirectX::SimpleMath::Vector3 playerWorldPos = mPlayerTransforms[i]->GetWorldPosition();
		DirectX::SimpleMath::Vector3 projPos = getProjPos(playerWorldPos);

		float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
		float height = GetScene()->GetScreenManager()->GetFixScreenHeight();
		float xViewport = ((projPos.x + 1) / 2) * width;
		float yViewport = ((1 - projPos.y) / 2) * height;

		//spdlog::trace("{}, {}", xViewport, yViewport);

		std::array<bool, 4> moveRestriction{};

		moveRestriction[static_cast<int>(physics::ERestrictDirection::PlusX)] = projPos.x >= mForbiddenAreaPaddingX.y;
		moveRestriction[static_cast<int>(physics::ERestrictDirection::PlusZ)] = projPos.y >= mForbiddenAreaPaddingY.y;
		moveRestriction[static_cast<int>(physics::ERestrictDirection::MinusX)] = projPos.x <= mForbiddenAreaPaddingX.x;
		moveRestriction[static_cast<int>(physics::ERestrictDirection::MinusZ)] = projPos.y <= mForbiddenAreaPaddingY.x;

		controller->SetMoveRestriction(moveRestriction);
	}
}

void fq::client::CameraMoving::InitCameraPos()
{
	return;

	// �÷��̾� Ʈ������ ���鼭 ���� �� ��� 
	std::vector<DirectX::SimpleMath::Vector3> playerSpawnPosList;
	playerSpawnPosList.push_back(GetScene()->GetObjectByName("PlayerSpawner1")->GetTransform()->GetWorldPosition());
	playerSpawnPosList.push_back(GetScene()->GetObjectByName("PlayerSpawner2")->GetTransform()->GetWorldPosition());
	playerSpawnPosList.push_back(GetScene()->GetObjectByName("PlayerSpawner3")->GetTransform()->GetWorldPosition());
	playerSpawnPosList.push_back(GetScene()->GetObjectByName("PlayerSpawner4")->GetTransform()->GetWorldPosition());

	DirectX::SimpleMath::Vector3 center = { 0, 0, 0 };

	for (const auto& spawnWorldPos : playerSpawnPosList)
	{
		center += spawnWorldPos;
	}

	center /= playerSpawnPosList.size();

	auto cameraTransform = GetTransform()->GetComponent<fq::game_module::Transform>();
	cameraTransform->SetLocalPosition(center);

	return;
}

// �ݶ��̴� �߰��ϱ�
// �ݶ��̴� �Ѿ �� ��� ó�� ?
