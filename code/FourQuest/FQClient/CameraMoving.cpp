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
	DirectX::SimpleMath::Vector3 center = GetCenterPoint(); // ���� ���ͷ� ���
	DirectX::SimpleMath::Vector3 centerCopy = center; // �Ÿ� ��� ��

	// 0,0 �ۿ� �ֵ��� �����̱� 
	float epsilon = 0.001f;
	if (abs(center.x) > epsilon || abs(center.y) > epsilon)
	{
		center.Normalize();
	}

	// center ��ġ�� ī�޶� ������ ������ �ݴ��̹Ƿ�

	// ī�޶��� ���� �� ���ϱ� 
	DirectX::SimpleMath::Matrix viewMatrix = mMainCamera->GetView();
	DirectX::SimpleMath::Vector3 cameraRight = { viewMatrix._11, viewMatrix._12, viewMatrix._13 };
	// �� ���� �ݴ���?
	DirectX::SimpleMath::Vector3 cameraUp = { viewMatrix._31, viewMatrix._32, viewMatrix._33 };
	DirectX::SimpleMath::Vector3 cameraForward = { viewMatrix._21, viewMatrix._22, viewMatrix._23 };

	// X ���� �̵� ��
	DirectX::SimpleMath::Vector3 rightMove{};
	// y ���� �̵� ��
	DirectX::SimpleMath::Vector3 upMove{};
	// z ���� �̵� ��
	DirectX::SimpleMath::Vector3 forwardMove{};

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

	// ī�޶� Ʈ�������� ����
	auto cameraTransform = mMainCamera->GetGameObject()->GetComponent<fq::game_module::Transform>();
	cameraTransform->SetLocalPosition(cameraTransform->GetLocalPosition() + rightMove + upMove + forwardMove);

	auto inputMgr = GetScene()->GetInputManager();
	if (inputMgr->IsKeyState(EKey::A, EKeyState::Tap))
	{
		//cameraTransform->SetLocalPosition(cameraTransform->GetLocalPosition() + cameraUp);
	}

	// Max, min ��
}

DirectX::SimpleMath::Vector3 fq::client::CameraMoving::GetCenterPoint() const
{
	// �÷��̾� Ʈ������ ��������
	std::vector<fq::game_module::Transform*> playersTransform;

	auto componentView = GetScene()->GetComponentView<Player>();
	for (auto& player : componentView)
	{
		playersTransform.push_back(player.GetComponent<fq::game_module::Transform>());
	}

	// width height ��� ������ 
	DirectX::SimpleMath::Matrix viewProjectionMatrix = mMainCamera->GetView();// * mMainCamera->GetProjection(2068.f/898.f);

	// ���� �� ��� 
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
