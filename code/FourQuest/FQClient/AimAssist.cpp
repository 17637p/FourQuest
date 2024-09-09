#include "AimAssist.h"

#include "../FQGameModule/Transform.h"

// Test
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/InputManager.h"


void fq::client::AimAssist::OnTriggerEnter(const game_module::Collision& collision)
{
	mInRangeMonsters.emplace_back(collision.other);
}

void fq::client::AimAssist::OnTriggerExit(const game_module::Collision& collision)
{
	mInRangeMonsters.remove(collision.other);
}

fq::client::AimAssist::~AimAssist()
{
}

fq::client::AimAssist::AimAssist()
	:mInRangeMonsters(),
	mTheta(180)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::AimAssist::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneAssist = std::dynamic_pointer_cast<AimAssist>(clone);

	if (cloneAssist == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneAssist = game_module::ObjectPool::GetInstance()->Assign<AimAssist>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneAssist = *this;
	}

	return cloneAssist;
}

void fq::client::AimAssist::SetNearMonsterDirection()
{
	auto playerTransform = GetTransform()->GetParentTransform();

	DirectX::SimpleMath::Vector3 forwardVec = playerTransform->GetLookAtVector();
	float minDistance = FLT_MAX;
	game_module::GameObject* minDistanceMonster = nullptr;
	// �浹�� ���� �߿��� ���� �� �ȿ� �ְ� ���� ����� ���� ã�� 
	for (auto monster : mInRangeMonsters)
	{
		DirectX::SimpleMath::Vector3 myPos = playerTransform->GetWorldPosition();
		DirectX::SimpleMath::Vector3 monsterPos = monster->GetTransform()->GetWorldPosition();

		DirectX::SimpleMath::Vector3 playerToMonsterVec = monsterPos - myPos;

		forwardVec.Normalize();
		playerToMonsterVec.Normalize();

		float cosTheta = playerToMonsterVec.Dot(forwardVec);
		if (cosTheta >= 0) // ���� 180�� 
		{
			float radian = std::acosf(cosTheta);
			float degree = radian * (180.0 / 3.1415926535f);

			if (degree <= (mTheta / 2)) 
			{
				float distance = DirectX::SimpleMath::Vector3::Distance(myPos, monsterPos);
				if (distance < minDistance)
				{
					minDistance = distance;
					minDistanceMonster = monster;
				}
			}
		}
	}

	// ȸ����Ű�� 
	if (minDistanceMonster != nullptr)
	{
		auto targetTransform = minDistanceMonster->GetComponent<game_module::Transform>();

		auto targetPos = targetTransform->GetWorldPosition();
		auto myPos = playerTransform->GetWorldPosition();

		auto directV = targetPos - myPos;
		directV.y = 0.f;
		directV.Normalize();

		constexpr float RotationSpeed = 0.1f;

		auto currentRotation = playerTransform->GetWorldRotation();
		DirectX::SimpleMath::Quaternion directionQuaternion = DirectX::SimpleMath::Quaternion::LookRotation(directV, { 0, 1, 0 });
		directionQuaternion.Normalize();
		//DirectX::SimpleMath::Quaternion result =
		//	DirectX::SimpleMath::Quaternion::Slerp(currentRotation, directionQuaternion, RotationSpeed);

		DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(directionQuaternion);

		// UpVector�� ������ ���
		if (rotationMatrix._22 <= -0.9f)
		{
			rotationMatrix._22 = 1.f;
		}
		playerTransform->SetLocalRotationToMatrix(rotationMatrix);
	}
}

void fq::client::AimAssist::OnUpdate(float dt)
{
	// Test �� 
	auto input = GetScene()->GetInputManager();
	if (input->GetKeyState(EKey::V) == EKeyState::Tap)
	{
		SetNearMonsterDirection();
	}
}

// Collision Matrix�� �����ϸ� ��
		//if (monster->GetTag() == game_module::ETag::Monster ||
		//	monster->GetTag() == game_module::ETag::Spawner)

// �ݶ��̴� �ޱ�
// transform lookat ���� ���� �������� ���� ��ġ - ����ġ ���� ���� �����ͼ� ���� �ϸ�
// ���� ���� ���� ���� 
// 
// �Ѵ� �����ϴ� ��� ���� ���� �����
// ����� ���� ã��
// 
// �÷��̾� ���� ȸ��